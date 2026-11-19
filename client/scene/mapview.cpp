/*
 * Copyright 2026 "tibiarc contributors"
 *
 * This file is part of tibiarc.
 *
 * tibiarc is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * tibiarc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with tibiarc. If not, see <https://www.gnu.org/licenses/>.
 */

#include "scene/mapview.hpp"

#include "assets/versions.hpp"
#include "scene/animation.hpp"
#include "scene/parts.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace trc {
namespace Client {

/* ------------------------------------------------------------------------ */

MapWindow::MapWindow(const Gamestate &gamestate)
    : State(gamestate),
      Center(gamestate.Map.Position),
      Floor(gamestate.Map.Position.Z <= 7 ? 7 - gamestate.Map.Position.Z
                                          : 2) {
}

Position MapWindow::Absolute(int column, int row, int level) const {
    const int offset = level - Floor;

    return Position(static_cast<uint16_t>(column + Center.X - 8 + offset),
                    static_cast<uint16_t>(row + Center.Y - 6 + offset),
                    static_cast<uint8_t>(Center.Z - offset));
}

bool MapWindow::Relative(const Position &position,
                         int &column,
                         int &row,
                         int &level) const {
    const int offset = static_cast<int>(Center.Z) - position.Z;

    column = static_cast<int>(position.X) + 8 - Center.X - offset;
    row = static_cast<int>(position.Y) + 6 - Center.Y - offset;
    level = Floor + offset;

    return level >= 0 && level < Levels;
}

const Tile *MapWindow::Field(int column, int row, int level) const {
    if (!Contains(column, row) || level < 0 || level >= Levels) {
        return nullptr;
    }

    const int offset = level - Floor;
    const int x = column + Center.X - 8 + offset;
    const int y = row + Center.Y - 6 + offset;
    const int z = static_cast<int>(Center.Z) - offset;

    /* Underground, only two floors in either direction are known. */
    if (x < 0 || y < 0 || z < 0 || z > 15 ||
        (Underground() && std::abs(offset) > 2) || (!Underground() && z > 7)) {
        return nullptr;
    }

    const Tile &tile = State.Map.Tile(x, y, z);

    return tile.ObjectCount > 0 ? &tile : nullptr;
}

/* ------------------------------------------------------------------------ */

namespace {

constexpr int MaxElevation = 24;
constexpr uint32_t PlayerIdLimit = 0x40000000;
constexpr int ShimmerEffect = 13;

/* The order in which a field's objects are drawn: ground, borders and
 * bottom objects as stacked, then everything else from the oldest to the
 * newest. */
struct DrawOrder {
    std::array<uint8_t, Tile::MaxObjects> Index;
    int Count = 0;

    DrawOrder(const Version &version, const Tile &tile) {
        int leading = 0;

        for (; leading < tile.ObjectCount; leading++) {
            const Object &object = tile.Objects[leading];

            if (object.IsCreature() ||
                version.GetItem(object.Id).Properties.StackPriority > 2) {
                break;
            }

            Index[Count++] = static_cast<uint8_t>(leading);
        }

        for (int i = tile.ObjectCount - 1; i >= leading; i--) {
            Index[Count++] = static_cast<uint8_t>(i);
        }
    }
};

} // namespace

/* ------------------------------------------------------------------------ */

struct MapRenderer::Pass {
    const Gamestate &State;
    const Version &Types;
    const MapWindow View;
    const uint32_t Now;
    const uint32_t Tick;
    Canvas &Target;
    MapFrame &Frame;
    OutfitColorizer &Colorizer;

    int ScrollX = 0, ScrollY = 0;

    /* Highest level at which opaque ground fills each field: sprites on
     * lower levels are not drawn there. */
    std::array<int8_t, MapWindow::Columns * MapWindow::Rows> Opaque;

    Pass(const Gamestate &gamestate,
         uint32_t now,
         Canvas &canvas,
         MapFrame &frame,
         OutfitColorizer &colorizer)
        : State(gamestate),
          Types(gamestate.Version),
          View(gamestate),
          Now(now),
          Tick(Clock::Ticks(now)),
          Target(canvas),
          Frame(frame),
          Colorizer(colorizer) {
    }

    const EntityType *ItemType(const Object &object) const {
        return object.IsCreature() ? nullptr : &Types.GetItem(object.Id);
    }

    const EntityType *FirstItem(int column, int row, int level) const {
        const Tile *tile = View.Field(column, row, level);

        return tile ? ItemType(tile->Objects[0]) : nullptr;
    }

    bool IsGround(const EntityType *type) const {
        return type != nullptr && type->Properties.StackPriority == 0;
    }

    template <typename Predicate>
    bool AnyItem(int column, int row, int level, Predicate &&predicate) const {
        if (const Tile *tile = View.Field(column, row, level)) {
            for (int i = 0; i < tile->ObjectCount; i++) {
                const EntityType *type = ItemType(tile->Objects[i]);

                if (type != nullptr && predicate(*type)) {
                    return true;
                }
            }
        }

        return false;
    }

    bool Translucent(int column, int row, int level) const {
        return AnyItem(column, row, level, [](const EntityType &type) {
            return type.Properties.Translucent;
        });
    }

    bool IsAtMost(int column, int row, int level) const {
        return MapWindow::Contains(column, row) &&
               Opaque[column * MapWindow::Rows + row] <= level;
    }

    /* -------------------------------------------------------------------- */

    /* Finds how many of the floors above the player can be seen, by looking
     * for something overhead on the player's own field and its four
     * neighbours that can be seen through. */
    int FindTopLevel() const {
        const int floor = View.PlayerLevel();
        int top = MapWindow::Levels - 1;

        for (int column = 7; column <= 9; column++) {
            for (int row = 5; row <= 7; row++) {
                const bool blocked =
                        AnyItem(column, row, floor, [](const EntityType &type) {
                            return type.Properties.BlockMissile;
                        });

                if ((column != 8 && row != 6) || blocked) {
                    continue;
                }

                for (int level = floor + 1;
                     level < MapWindow::Levels &&
                     column - level + floor >= 0 && row - level + floor >= 0;
                     level++) {
                    if (level - 1 >= top) {
                        continue;
                    }

                    /* The field straight above ... */
                    const EntityType *above = FirstItem(column - level + floor,
                                                        row - level + floor,
                                                        level);

                    if (IsGround(above) && !above->Properties.DontHide) {
                        top = level - 1;
                    }

                    /* ... and the one that covers it on screen. */
                    const EntityType *cover = FirstItem(column, row, level);

                    if (cover != nullptr && level - 1 < top &&
                        cover->Properties.StackPriority != 1 &&
                        cover->Properties.StackPriority <= 2 &&
                        !cover->Properties.DontHide) {
                        top = level - 1;
                    }
                }
            }
        }

        return top;
    }

    void SetLight(int column, int row, int brightness) {
        const auto ambient = State.Map.Light.At(Now);

        brightness = std::clamp(brightness, 0, 255);

        Frame.Light.At(column, row) =
                LightMap::Color{ambient.Red * brightness / 255,
                                ambient.Green * brightness / 255,
                                ambient.Blue * brightness / 255};
    }

    /* Brightens the fields around a light source at the given map pixel. */
    void AddLight(int x, int y, int level, int radius, LightMap::Color color) {
        if (radius <= 0) {
            return;
        }

        x += ScrollX;
        y += ScrollY;

        for (int column = x / 32 - radius + 1; column < x / 32 + radius;
             column++) {
            for (int row = y / 32 - radius + 1; row < y / 32 + radius; row++) {
                if (!MapWindow::Contains(column, row)) {
                    continue;
                }

                const int dx = column * 32 - x + 16;
                const int dy = row * 32 - y + 16;
                const double distance = std::sqrt(
                        static_cast<double>(dx) * dx +
                        static_cast<double>(dy) * dy);
                const int strength = radius * 32 - static_cast<int>(distance);

                LightMap::Color light{color.Red * strength / 160,
                                      color.Green * strength / 160,
                                      color.Blue * strength / 160};

                /* Light fades where there's no floor to fall on. */
                if (!IsGround(FirstItem(column, row, level))) {
                    light = light.Scale(4, 5);
                }

                light.Red = std::clamp(light.Red, 0, 255);
                light.Green = std::clamp(light.Green, 0, 255);
                light.Blue = std::clamp(light.Blue, 0, 255);

                auto &field = Frame.Light.At(column, row);
                field = field.Max(light);
            }
        }
    }

    void AddLight(int x, int y, int level, const EntityType &type) {
        if (type.Properties.LightIntensity > 0) {
            AddLight(x,
                     y,
                     level,
                     type.Properties.LightIntensity,
                     LightMap::Color::FromPalette(type.Properties.LightColor));
        }
    }

    /* -------------------------------------------------------------------- */

    /* Draws a sprite at a map pixel. Ordinary sprites are drawn field by
     * field, skipping fields that opaque ground on a higher level will
     * cover anyway; `whole` sprites are drawn in full as soon as any field
     * they touch is visible. */
    void PutSprite(int x, int y, int level, bool whole, const Sprite &sprite) {
        const int mapX = ScrollX + x, mapY = ScrollY + y;
        const int column = (mapX + 32) / 32 - 1;
        const int row = (mapY + 32) / 32 - 1;
        const bool spansColumns = (mapX & 31) != 0;
        const bool spansRows = (mapY & 31) != 0;

        const std::array<std::pair<int, int>, 4> fields = {
                {{column, row},
                 {column + 1, row},
                 {column, row + 1},
                 {column + 1, row + 1}}};
        const std::array<bool, 4> touched = {true,
                                             spansColumns,
                                             spansRows,
                                             spansColumns && spansRows};

        if (whole) {
            for (int i = 0; i < 4; i++) {
                if (touched[i] &&
                    IsAtMost(fields[i].first, fields[i].second, level)) {
                    Target.Draw(sprite, x, y);
                    return;
                }
            }

            return;
        }

        for (int i = 0; i < 4; i++) {
            if (touched[i] &&
                IsAtMost(fields[i].first, fields[i].second, level)) {
                ClipScope clip(Target,
                               Rect{fields[i].first * 32 - ScrollX,
                                    fields[i].second * 32 - ScrollY,
                                    32,
                                    32});
                Target.Draw(sprite, x, y);
            }
        }
    }

    /* Draws every part and layer of one picture of a type. */
    void PutType(const EntityType::FrameGroup &group,
                 int x,
                 int y,
                 int level,
                 bool whole,
                 int patternX,
                 int patternY,
                 int patternZ,
                 int phase) {
        for (int partX = 0; partX < group.SizeX; partX++) {
            for (int partY = 0; partY < group.SizeY; partY++) {
                for (int layer = 0; layer < group.LayerCount; layer++) {
                    if (const Sprite *sprite = PartSprite(group,
                                                          partX,
                                                          partY,
                                                          layer,
                                                          patternX,
                                                          patternY,
                                                          patternZ,
                                                          phase)) {
                        PutSprite(x - partX * 32,
                                  y - partY * 32,
                                  level,
                                  whole,
                                  *sprite);
                    }
                }
            }
        }
    }

    void DrawObject(int x,
                    int y,
                    int level,
                    const Object &object,
                    const EntityType &type,
                    const Position &position,
                    int hook) {
        const auto &group = DefaultGroup(type);
        const auto &properties = type.Properties;

        int patternX = position.X % group.XDiv;
        int patternY = position.Y % group.YDiv;
        int patternZ = position.Z % group.ZDiv;
        int phase;

        if (Types.Features.AnimationPhases) {
            phase = object.Animation % group.FrameCount;
        } else {
            phase = (Tick / 5) % group.FrameCount;
        }

        if (properties.Stackable) {
            const int picture = StackPicture(object.ExtraByte);

            patternX = (picture % 4) % group.XDiv;
            patternY = (picture / 4) % group.YDiv;
        }

        if (properties.LiquidPool || properties.LiquidContainer) {
            const int liquid = Types.TranslateFluidColor(object.ExtraByte);

            patternX = (liquid % 4) % group.XDiv;
            patternY = (liquid / 4) % group.YDiv;
        }

        if (properties.Hangable) {
            patternX = (hook < group.XDiv) ? hook : 0;
            patternY = 0;
        }

        PutType(group,
                x - properties.DisplacementX,
                y - properties.DisplacementY,
                level,
                group.FrameCount > 1,
                patternX,
                patternY,
                patternZ,
                phase);
    }

    void DrawOutfit(int x,
                    int y,
                    int level,
                    const Creature &creature,
                    const EntityType &type,
                    const WalkPose &pose,
                    bool mounted) {
        const bool moving = pose.OffsetX != 0 || pose.OffsetY != 0 ||
                            Now <= creature.Walk.EndTick;
        const auto &group = type.FrameGroups[std::to_underlying(
                (moving && creature.Walk.EndTick != 0)
                        ? FrameGroupIndex::Walking
                        : FrameGroupIndex::Idle)];
        const int direction = std::to_underlying(pose.Heading);
        int phase;

        if (type.Properties.AnimateIdle) {
            phase = (Tick / 3) % group.FrameCount;
        } else {
            phase = pose.Phase(group.FrameCount, Now > creature.Walk.EndTick);
        }

        /* A rider is drawn on top of its mount without any displacement of
         * its own. */
        if (!mounted) {
            x -= type.Properties.DisplacementX;
            y -= type.Properties.DisplacementY;
        }

        for (int addon = 0; addon < group.YDiv; addon++) {
            if (addon > 0 && !(creature.Outfit.Addons & (1 << (addon - 1)))) {
                continue;
            }

            for (int partX = 0; partX < group.SizeX; partX++) {
                for (int partY = 0; partY < group.SizeY; partY++) {
                    const Sprite *sprite = PartSprite(group,
                                                      partX,
                                                      partY,
                                                      0,
                                                      direction,
                                                      addon,
                                                      mounted ? 1 : 0,
                                                      phase);

                    if (sprite == nullptr) {
                        continue;
                    }

                    if (group.LayerCount >= 2) {
                        if (const Sprite *mask = PartSprite(group,
                                                            partX,
                                                            partY,
                                                            1,
                                                            direction,
                                                            addon,
                                                            mounted ? 1 : 0,
                                                            phase)) {
                            sprite = &Colorizer.Get(*sprite,
                                                    *mask,
                                                    creature.Outfit);
                        }
                    }

                    PutSprite(x - partX * 32,
                              y - partY * 32,
                              level,
                              true,
                              *sprite);
                }
            }
        }
    }

    /* -------------------------------------------------------------------- */

    struct Standing {
        const Creature *Who;
        WalkPose Pose;
        int DX, DY;
        int ShiftX, ShiftY;
        int Column, Row;
    };

    /* Collects the creatures on this field and its neighbours whose picture
     * starts on this field, back to front. */
    int CollectCreatures(int column,
                         int row,
                         int level,
                         std::array<Standing, 10> &found) const {
        int count = 0;

        for (int x = column - 1; x <= column + 1; x++) {
            for (int y = row - 1; y <= row + 1; y++) {
                const Tile *tile = View.Field(x, y, level);

                if (tile == nullptr) {
                    continue;
                }

                const DrawOrder order(Types, *tile);

                for (int i = 0; i < order.Count && count < 10; i++) {
                    const Object &object = tile->Objects[order.Index[i]];

                    if (!object.IsCreature()) {
                        continue;
                    }

                    const Creature *creature =
                            State.FindCreature(object.CreatureId);

                    if (creature == nullptr) {
                        continue;
                    }

                    int shiftX = 0, shiftY = 0;

                    if (creature->Outfit.Id == 0) {
                        if (creature->Outfit.Item.Id == 0) {
                            /* Invisible: only players leave a shimmer. */
                            if (creature->Id >= PlayerIdLimit) {
                                continue;
                            }

                            shiftX = shiftY = 8;
                        } else {
                            const auto &properties =
                                    Types.GetItem(creature->Outfit.Item.Id)
                                            .Properties;

                            shiftX = properties.DisplacementX;
                            shiftY = properties.DisplacementY;
                        }
                    } else {
                        const auto &properties =
                                Types.GetOutfit(creature->Outfit.Id)
                                        .Properties;

                        shiftX = properties.DisplacementX;
                        shiftY = properties.DisplacementY;
                    }

                    const WalkPose pose = EvaluateWalk(*creature, Now);
                    const int dx = (x - column) * 32 - shiftX + pose.OffsetX;
                    const int dy = (y - row) * 32 - shiftY + pose.OffsetY;

                    if (dx >= -31 && dx <= 0 && dy >= -31 && dy <= 0) {
                        found[count++] = Standing{creature,
                                                  pose,
                                                  dx,
                                                  dy,
                                                  shiftX,
                                                  shiftY,
                                                  x,
                                                  y};
                    }
                }
            }
        }

        /* By row, and from the left within a row. The exchange sort is
         * kept as is since it decides the order of creatures that tie. */
        for (int i = 0; i < count - 1; i++) {
            for (int j = i + 1; j < count; j++) {
                if (found[i].DY < found[j].DY ||
                    (found[i].DY == found[j].DY &&
                     found[i].DX < found[j].DX)) {
                    continue;
                }

                std::swap(found[i], found[j]);
            }
        }

        return count;
    }

    bool ShowsName(const Standing &standing, int level) const {
        const Creature &creature = *standing.Who;

        if (level != View.PlayerLevel()) {
            return false;
        }

        if (creature.Id == State.Player.Id) {
            return true;
        }

        if (creature.Health < 1 ||
            (creature.Id >= PlayerIdLimit && creature.Outfit.Id == 0 &&
             creature.Outfit.Item.Id == 0)) {
            return false;
        }

        return std::abs(standing.Column - 8) <= 7 &&
               std::abs(standing.Row - 6) <= 5;
    }

    void DrawCreature(int x, int y, int level, const Standing &standing) {
        const Creature &creature = *standing.Who;
        const int left = standing.DX + x, top = standing.DY + y;

        if (creature.SquareEndTick != 0 && creature.SquareEndTick >= Now) {
            /* Coloured marks are drawn a little inside the field so they
             * stay apart from the black one. */
            const int inset = creature.SquareColor == 0 ? 0 : 2;

            Target.Frame(Rect{left + inset,
                              top + inset,
                              32 - inset * 2,
                              32 - inset * 2},
                         2,
                         Pixel::ProtocolColor(creature.SquareColor));
        }

        if (creature.Outfit.Id == 0) {
            if (creature.Outfit.Item.Id == 0) {
                const auto &group =
                        DefaultGroup(Types.GetEffect(ShimmerEffect));

                if (const Sprite *sprite = PartSprite(group,
                                                      0,
                                                      0,
                                                      0,
                                                      0,
                                                      0,
                                                      0,
                                                      (Tick / 3) % 3 + 1)) {
                    Target.Draw(*sprite,
                                left + standing.ShiftX - 8,
                                top + standing.ShiftY - 8);
                }
            } else {
                Object look(creature.Outfit.Item.Id);
                look.ExtraByte = creature.Outfit.Item.ExtraByte;

                DrawObject(left + standing.ShiftX,
                           top + standing.ShiftY,
                           level,
                           look,
                           Types.GetItem(look.Id),
                           Position(0, 0, 0),
                           0);
            }
        } else {
            const int drawX = left + standing.ShiftX;
            const int drawY = top + standing.ShiftY;

            if (creature.Outfit.MountId != 0) {
                DrawOutfit(drawX,
                           drawY,
                           level,
                           creature,
                           Types.GetOutfit(creature.Outfit.MountId),
                           standing.Pose,
                           false);
                DrawOutfit(drawX,
                           drawY,
                           level,
                           creature,
                           Types.GetOutfit(creature.Outfit.Id),
                           standing.Pose,
                           true);
            } else {
                DrawOutfit(drawX,
                           drawY,
                           level,
                           creature,
                           Types.GetOutfit(creature.Outfit.Id),
                           standing.Pose,
                           false);
            }
        }
    }

    void CollectCreature(int x, int y, int level, const Standing &standing) {
        const Creature &creature = *standing.Who;
        const int left = standing.DX + x, top = standing.DY + y;

        if (ShowsName(standing, level)) {
            Frame.Creatures.push_back(
                    MapFrame::CreatureMark{creature.Id,
                                           left + 16,
                                           top,
                                           standing.Column,
                                           standing.Row,
                                           level});
        }

        AddLight(left + 16,
                 top + 16,
                 level,
                 creature.LightIntensity,
                 LightMap::Color::FromPalette(creature.LightColor));

        if (creature.Outfit.Id != 0) {
            AddLight(left + 16,
                     top + 16,
                     level,
                     Types.GetOutfit(creature.Outfit.Id));
        }

        /* The player always sees a little of what's around them. */
        if (creature.Id == State.Player.Id && creature.LightIntensity < 2) {
            AddLight(left + 16,
                     top + 16,
                     level,
                     2,
                     LightMap::Color{255, 255, 255});
        }
    }

    /* -------------------------------------------------------------------- */

    void DrawEffects(int x,
                     int y,
                     int column,
                     int row,
                     int level,
                     const Tile *tile,
                     bool collect) {
        /* Missiles belong to the field they are currently flying over. */
        if (level == View.PlayerLevel()) {
            const Position here = View.Absolute(column, row, level);

            for (const Missile &missile : State.MissileList) {
                if (missile.Id == 0 || missile.Origin.Z != here.Z ||
                    missile.Target.Z != here.Z) {
                    continue;
                }

                const MissilePose pose = EvaluateMissile(missile, Now);

                if (!pose.Alive) {
                    continue;
                }

                const int cellX = (missile.Target.X * 32 + 31 + pose.OffsetX) /
                                  32;
                const int cellY = (missile.Target.Y * 32 + 31 + pose.OffsetY) /
                                  32;

                if (cellX != here.X || cellY != here.Y) {
                    continue;
                }

                static constexpr int Patterns[8][2] = {{1, 0},
                                                       {2, 0},
                                                       {2, 1},
                                                       {2, 2},
                                                       {1, 2},
                                                       {0, 2},
                                                       {0, 1},
                                                       {0, 0}};
                const EntityType &type = Types.GetMissile(missile.Id);
                const int flightX = (missile.Target.X - cellX) * 32 +
                                    pose.OffsetX + x;
                const int flightY = (missile.Target.Y - cellY) * 32 +
                                    pose.OffsetY + y;

                PutType(DefaultGroup(type),
                        flightX,
                        flightY,
                        level,
                        true,
                        Patterns[pose.Direction][0],
                        Patterns[pose.Direction][1],
                        0,
                        0);

                if (collect) {
                    AddLight(flightX + 16, flightY + 16, level, type);
                }
            }
        }

        if (tile == nullptr) {
            return;
        }

        /* Newest first. */
        struct Entry {
            uint32_t Sequence;
            const GraphicalEffect *Graphical;
            const NumericalEffect *Numerical;
        };

        std::array<Entry, Tile::MaxEffects * 2> entries;
        int count = 0;

        for (const auto &effect : tile->GraphicalEffects) {
            if (effect.Id != 0) {
                entries[count++] = Entry{effect.Sequence, &effect, nullptr};
            }
        }

        for (const auto &effect : tile->NumericalEffects) {
            if (effect.Active) {
                entries[count++] = Entry{effect.Sequence, nullptr, &effect};
            }
        }

        std::sort(entries.begin(),
                  entries.begin() + count,
                  [](const Entry &a, const Entry &b) {
                      return a.Sequence > b.Sequence;
                  });

        for (int i = 0; i < count; i++) {
            if (const GraphicalEffect *effect = entries[i].Graphical) {
                const EntityType &type = Types.GetEffect(effect->Id);
                const auto &group = DefaultGroup(type);
                const int phase = Clock::TicksSince(Now, effect->StartTick);

                if (Now < effect->StartTick || phase >= group.FrameCount) {
                    continue;
                }

                PutType(group, x, y, level, false, 0, 0, 0, phase);

                if (collect && type.Properties.LightIntensity > 0) {
                    const int radius =
                            std::min({phase + 1,
                                      group.FrameCount - phase,
                                      static_cast<int>(
                                              type.Properties.LightIntensity)});

                    AddLight(x + 16,
                             y + 16,
                             level,
                             radius,
                             LightMap::Color::FromPalette(
                                     type.Properties.LightColor));
                }
            } else if (collect) {
                const NumericalEffect *effect = entries[i].Numerical;
                const int age = Clock::TicksSince(Now, effect->StartTick);

                if (Now < effect->StartTick || age > 10) {
                    continue;
                }

                Frame.Texts.push_back(MapFrame::TextMark{x + 16,
                                                         y - age * 2,
                                                         column,
                                                         row,
                                                         level,
                                                         effect->Color,
                                                         effect->Value});
            }
        }
    }

    /* One field of the view: its objects bottom up, the creatures standing
     * on it, its effects, and finally whatever is always on top. `redraw`
     * repaints a neighbour over an object that reaches into it, skipping
     * the neighbour's own objects. */
    void DrawField(int x, int y, int column, int row, int level, bool redraw) {
        const Tile *tile = View.Field(column, row, level);
        const Position position = View.Absolute(column, row, level);

        int elevation = 0;
        bool overlaps = false;
        const Object *top = nullptr;

        if (tile != nullptr) {
            const DrawOrder order(Types, *tile);
            int hook = 0;

            /* Something to hang on decides how hangable objects look. */
            for (int i = 0; i < tile->ObjectCount; i++) {
                if (const EntityType *type = ItemType(tile->Objects[i])) {
                    if (type->Properties.Vertical) {
                        hook = 1;
                        break;
                    } else if (type->Properties.Horizontal) {
                        hook = 2;
                        break;
                    }
                }
            }

            for (int i = 0; i < order.Count; i++) {
                const Object &object = tile->Objects[order.Index[i]];
                const EntityType *type = ItemType(object);

                if (type == nullptr) {
                    continue;
                }

                if (!redraw) {
                    AddLight(x - type->Properties.DisplacementX - elevation +
                                     16,
                             y - type->Properties.DisplacementY - elevation +
                                     16,
                             level,
                             *type);
                }

                if (type->Properties.StackPriority == 3) {
                    top = &object;
                } else if (!redraw) {
                    DrawObject(x - elevation,
                               y - elevation,
                               level,
                               object,
                               *type,
                               position,
                               hook);

                    overlaps = overlaps || type->Properties.RedrawNearbyTop;
                }

                elevation = std::min<int>(MaxElevation,
                                          elevation + type->Properties.Height);
            }
        }

        if (overlaps) {
            if (column > 0) {
                if (row > 0) {
                    DrawField(x - 32, y - 32, column - 1, row - 1, level, true);
                }

                DrawField(x - 32, y, column - 1, row, level, true);
            }

            if (row > 0) {
                DrawField(x, y - 32, column, row - 1, level, true);
            }
        }

        std::array<Standing, 10> creatures;
        const int count = CollectCreatures(column, row, level, creatures);

        for (int i = 0; i < count; i++) {
            DrawCreature(x - elevation, y - elevation, level, creatures[i]);

            if (!redraw) {
                CollectCreature(x - elevation,
                                y - elevation,
                                level,
                                creatures[i]);
            }
        }

        DrawEffects(x - elevation,
                    y - elevation,
                    column,
                    row,
                    level,
                    tile,
                    !redraw);

        if (top != nullptr) {
            DrawObject(x,
                       y,
                       level,
                       *top,
                       Types.GetItem(top->Id),
                       position,
                       0);
        }
    }

    /* -------------------------------------------------------------------- */

    void Run() {
        const Creature *player = State.FindCreature(State.Player.Id);

        Frame.Creatures.clear();
        Frame.Texts.clear();
        Frame.Valid = false;

        Target.Fill(Target.Bounds(), Pixel(0, 0, 0));

        if (player == nullptr) {
            return;
        }

        const WalkPose pose = EvaluateWalk(*player, Now);
        const int floor = View.PlayerLevel();

        ScrollX = pose.OffsetX + 32;
        ScrollY = pose.OffsetY + 32;

        Frame.PlayerOffsetX = pose.OffsetX;
        Frame.PlayerOffsetY = pose.OffsetY;
        Frame.PlayerLevel = floor;
        Frame.TopLevel = FindTopLevel();

        for (int column = 0; column < MapWindow::Columns; column++) {
            for (int row = 0; row < MapWindow::Rows; row++) {
                int8_t opaque = 0, roof = -1;

                for (int level = Frame.TopLevel; level > 0; level--) {
                    const EntityType *first = FirstItem(column, row, level);

                    if (roof < 0 && IsGround(first)) {
                        roof = static_cast<int8_t>(level);
                    }

                    if (first != nullptr && first->Properties.FullGround) {
                        opaque = static_cast<int8_t>(level);
                        break;
                    }
                }

                if (roof < 0 && IsGround(FirstItem(column, row, 0))) {
                    roof = 0;
                }

                Opaque[column * MapWindow::Rows + row] = opaque;
                Frame.Roof[column * MapWindow::Rows + row] = roof;
            }
        }

        const int ambient =
                View.Underground() ? 0 : State.Map.Light.At(Now).Level;
        const int daylight = State.Map.Light.At(Now).Level;

        for (int level = 0; level <= Frame.TopLevel; level++) {
            if (level != 0 && level == floor) {
                Frame.Light.Dim();
            }

            for (int column = 0; column < MapWindow::Columns; column++) {
                for (int row = 0; row < MapWindow::Rows; row++) {
                    /* A floor shuts out the light from below, unless it can
                     * be seen through. */
                    if (level == 0 || IsGround(FirstItem(column, row, level))) {
                        const LightMap::Color below =
                                Frame.Light.At(column, row);

                        SetLight(column, row, ambient);

                        if (level > 0 && Translucent(column, row, level)) {
                            auto &field = Frame.Light.At(column, row);
                            field = field.Max(below.Scale(4, 5));
                        }
                    }

                    /* Daylight falls through openings onto the first floor
                     * below ground. */
                    if (level == State.Map.Position.Z + floor - 8 &&
                        level < 7 && column > 0 && row > 0 &&
                        Translucent(column - 1, row - 1, level + 1)) {
                        SetLight(column, row, daylight - 1);
                    }
                }
            }

            for (int column = 0; column < MapWindow::Columns; column++) {
                for (int row = 0; row < MapWindow::Rows; row++) {
                    DrawField(column * 32 - ScrollX,
                              row * 32 - ScrollY,
                              column,
                              row,
                              level,
                              false);
                }
            }
        }

        Frame.Light.Shade(Target, ScrollX, ScrollY);
        Frame.Valid = true;
    }
};

void MapRenderer::Render(const Gamestate &gamestate,
                         uint32_t now,
                         Canvas &canvas,
                         MapFrame &frame) {
    Pass pass(gamestate, now, canvas, frame, Colorizer);

    pass.Run();
}

} // namespace Client
} // namespace trc
