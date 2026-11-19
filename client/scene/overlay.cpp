/*
 * Copyright 2026 "Juan Pinheiro"
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

#include "scene/overlay.hpp"

#include "scene/health.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace trc {
namespace Client {

namespace {

/* Keeps a value between two bounds, whichever of them is the larger. */
void Bound(int &value, int a, int b) {
    value = std::clamp(value, std::min(a, b), std::max(a, b));
}

struct Projection {
    const Rect &View;
    bool Cropped;

    /* Map pixels to pixels of the game window. */
    void Apply(int &x, int &y) const {
        if (Cropped) {
            x -= (View.Width < MapFrame::Width)
                         ? (MapFrame::Width - View.Width) / 2
                         : 0;
            y -= (View.Height < MapFrame::Height)
                         ? (MapFrame::Height - View.Height) / 2
                         : 0;
        } else {
            x = x * View.Width / MapFrame::Width;
            y = y * View.Height / MapFrame::Height;
        }
    }
};

} // namespace

void MapOverlay::Draw(Canvas &canvas,
                      const Rect &view,
                      const MapFrame &frame,
                      const Gamestate &gamestate,
                      const World &world,
                      const Options &options,
                      uint32_t now) {
    if (!frame.Valid) {
        return;
    }

    ClipScope clip(canvas, view);

    DrawCreatures(canvas, view, frame, gamestate, options);
    DrawTalk(canvas, view, frame, gamestate, world, options, now);
    DrawTexts(canvas, view, frame, options);
}

void MapOverlay::DrawCreatures(Canvas &canvas,
                               const Rect &view,
                               const MapFrame &frame,
                               const Gamestate &gamestate,
                               const Options &options) {
    if (options.ShowCreatureInfo == Options::CreatureInfo::None) {
        return;
    }

    const Font &font = Text.Game;
    const Projection projection{view, options.NoStretching};
    const Creature *player = gamestate.FindCreature(gamestate.Player.Id);

    for (const auto &mark : frame.Creatures) {
        const Creature *creature = gamestate.FindCreature(mark.Id);

        if (creature == nullptr) {
            continue;
        }

        const bool inside = MapWindow::Contains(mark.Column, mark.Row);
        const bool visible = inside &&
                             !frame.Covered(mark.Column, mark.Row, mark.Level);

        /* Names fade with the light their creature stands in, though the
         * player's own never goes fully dark. */
        Pixel color = visible ? HealthColor(creature->Health)
                              : Pixel(0xC0, 0xC0, 0xC0);
        int brightness = inside ? frame.Light.At(mark.Column, mark.Row).Red : 0;

        if (creature->Id == gamestate.Player.Id) {
            const int glow = std::clamp<int>(creature->LightIntensity, 2, 5);

            brightness = std::max(brightness, glow * 51);
        }

        color = Pixel(color.Red * (brightness / 5) / 51,
                      color.Green * (brightness / 5) / 51,
                      color.Blue * (brightness / 5) / 51);

        int x = mark.X, y = mark.Y;
        projection.Apply(x, y);

        int barX = x - 14, barY = y - 4;

        Bound(barX, 1, view.Width - 28);

        if (options.ShowCreatureInfo == Options::CreatureInfo::NamesAndBars) {
            const int textWidth = font.TextWidth(creature->Name);

            Bound(barY,
                  font.LineHeight() + 1,
                  view.Height - font.LineHeight() - 5);

            int textX = barX - textWidth / 2 + 14;
            const int textY = barY - font.LineHeight();

            Bound(textX, 1, view.Width - textWidth - 1);

            font.Draw(canvas,
                      view.X + textX,
                      view.Y + textY,
                      color,
                      creature->Name);
        } else {
            Bound(barY, 1, view.Height - 5);
        }

        canvas.Fill(Rect{view.X + barX, view.Y + barY, 27, 4}, Pixel(0, 0, 0));
        canvas.Fill(Rect{view.X + barX + 1,
                         view.Y + barY + 1,
                         creature->Health / 4,
                         2},
                    color);

        /* Party and skull marks. */
        static constexpr int PartySkins[] = {0, 0xFC, 0xFD, 0xFE, 0xFF};
        static constexpr int SkullSkins[] = {0, 0xF4, 0xF5, 0xF6, 0xF7};

        const auto shield = std::to_underlying(creature->Shield);
        const auto skull = std::to_underlying(creature->Skull);
        int first = 0, second = 0;

        if (shield >= 1 && shield <= 4) {
            const bool invited = (shield == 1);
            const bool playerInParty =
                    player != nullptr && player->Shield != PartyShield::None;

            if (!invited || !playerInParty) {
                first = PartySkins[shield];
            }
        }

        if (skull >= 1 && skull <= 4) {
            if (first == 0) {
                first = SkullSkins[skull];
            } else {
                second = SkullSkins[skull];
            }
        }

        if (first != 0) {
            const Rect &area = Skin::Area(first);
            int markX = x - area.Width + 20, markY = y + 1;

            Bound(markX,
                  1,
                  view.Width - area.Width - (second != 0 ? 14 : 1));
            Bound(markY, 1, view.Height - area.Height - 1);

            Pieces.Draw(canvas, first, view.X + markX, view.Y + markY);

            if (second != 0) {
                Pieces.Draw(canvas,
                            second,
                            view.X + markX + 13,
                            view.Y + markY);
            }
        }
    }
}

void MapOverlay::PlaceTalk(const Rect &view,
                           const Gamestate &gamestate,
                           const World &world,
                           const Options &options,
                           bool statusShown) {
    struct Box {
        int TextWidth, TextHeight;
        int MinX, MaxX, MinY, MaxY;
        int FirstX;
        int X, Y;
        int PushX, PushY;
        bool Centred;
    };

    const Font &font = Text.Game;
    const MapWindow window(gamestate);
    const int width = view.Width, height = view.Height;
    const int margin = statusShown ? font.LineHeight() + 10 : 0;

    std::vector<Box> boxes;

    for (const Talkbox &talk : world.Talk.Boxes()) {
        if (!talk.Shown) {
            continue;
        }

        Box &box = boxes.emplace_back();
        const int lines = static_cast<int>(talk.Lines.size());

        box.TextWidth = 0;

        for (const auto &line : talk.Lines) {
            box.TextWidth = std::max(box.TextWidth, font.TextWidth(line));
        }

        box.TextHeight = font.LinePitch() * lines;

        const int halfWidth = box.TextWidth / 2;
        const int halfHeight = box.TextHeight / 2;
        int centerX, centerY;

        if (talk.Positioned) {
            int column, row, level;

            window.Relative(talk.Position, column, row, level);

            if (options.NoStretching) {
                centerX = column * 32 - (MapFrame::Width - width) / 2 - 16;
                centerY = row * 32 - (MapFrame::Height - height) / 2 - 32;
            } else {
                centerX = (column - 1) * width / 15 + width / 15 / 2;
                centerY = (row - 1) * height / 11;
            }

            /* The text ends where the field it was said on begins. */
            centerY -= font.LinePitch() * lines / 2;

            box.MinY = std::max(halfHeight + 1, centerY);
            box.Centred = false;
        } else {
            centerX = width / 2;

            switch (talk.Kind) {
            case TalkKind::Info:
                centerY = height / 2 - box.TextHeight / 2 - 5;
                break;
            case TalkKind::Private:
                centerY = height / 4;
                break;
            default:
                centerY = box.TextHeight / 2 + 5 + height / 2;
                break;
            }

            box.MinY = std::max(halfHeight + 1, centerY - box.TextHeight);
            box.Centred = true;
        }

        box.FirstX = centerX;
        box.MinX = std::max(halfWidth + 1, centerX - halfWidth);
        box.MaxX = std::min(width - halfWidth, centerX + halfWidth);
        box.MaxY = std::min(height - halfHeight - margin,
                            centerY + box.TextHeight);

        box.X = centerX;
        box.Y = centerY;

        Bound(box.X, box.MinX, box.MaxX);
        Bound(box.Y, box.MinY, box.MaxY);
        Bound(box.X, halfWidth + 1, width - halfWidth);
        Bound(box.Y, halfHeight + 1, height - halfHeight - margin);
    }

    /* Nudge overlapping texts apart, a pixel at a time, for as long as that
     * keeps reducing how much they overlap. */
    const int count = static_cast<int>(boxes.size());
    int64_t overlap = INT64_MAX, last;

    do {
        last = overlap;
        overlap = 0;

        for (int i = 0; i < count; i++) {
            Box &box = boxes[i];

            box.PushX = box.PushY = 0;

            for (int j = 0; j < count; j++) {
                const Box &other = boxes[j];

                if (i == j) {
                    continue;
                }

                const int dx = other.X - box.X, dy = other.Y - box.Y;
                const int ax = std::abs(dx), ay = std::abs(dy);
                const int halfI = box.TextWidth / 2;
                const int halfJ = other.TextWidth / 2;
                const int highI = box.TextHeight / 2;
                const int highJ = other.TextHeight / 2;

                if (ax >= halfI + halfJ + 10 || ay >= highI + highJ + 10) {
                    continue;
                }

                int ox, oy;

                if (halfJ < halfI + ax) {
                    ox = (halfI < halfJ + ax) ? halfI + halfJ - ax + 10
                                              : other.TextWidth + 20;
                } else {
                    ox = box.TextWidth + 20;
                }

                if (highJ < highI + ay) {
                    oy = (highI < highJ + ay) ? highI + highJ - ay + 10
                                              : other.TextHeight + 20;
                } else {
                    oy = box.TextHeight + 20;
                }

                overlap += static_cast<int64_t>(oy) * ox;

                box.PushX += (dx < 0) ? ox : (dx > 0 ? -ox : 0);
                box.PushY += (dy < 0) ? oy : (dy > 0 ? -oy : 0);

                if (dx == 0 && dy == 0) {
                    box.PushX += i - j;
                }
            }
        }

        for (Box &box : boxes) {
            if (box.PushX < 0 && box.MinX < box.X) {
                box.X--;
            }

            if (box.PushX > 0 && box.X < box.MaxX) {
                box.X++;
            }

            if (box.PushY < 0 && box.MinY < box.Y) {
                box.Y--;
            }

            if (box.PushY > 0 && box.Y < box.MaxY) {
                box.Y++;
            }
        }
    } while (overlap < last);

    Placements.clear();

    for (const Box &box : boxes) {
        const int halfWidth = box.TextWidth / 2;
        const int halfHeight = box.TextHeight / 2;
        const int quarter = box.TextWidth / 4;
        Placement placement;

        placement.PinnedX = box.Centred || box.X == halfWidth + 1 ||
                            box.X == width - halfWidth;
        placement.PinnedY = box.Centred || box.Y == halfHeight + 1 ||
                            box.Y == height - halfHeight - margin;
        placement.Y = box.Y - halfHeight;

        /* Text that was pushed well aside hugs the side it came from. */
        if (box.X < box.FirstX - quarter) {
            placement.X = box.X + halfWidth;
            placement.Alignment = Align::Right;
        } else if (box.FirstX + quarter < box.X) {
            placement.X = box.X - halfWidth;
            placement.Alignment = Align::Left;
        } else {
            placement.X = box.X;
            placement.Alignment = Align::Center;
        }

        Placements.push_back(placement);
    }
}

void MapOverlay::DrawTalk(Canvas &canvas,
                          const Rect &view,
                          const MapFrame &frame,
                          const Gamestate &gamestate,
                          const World &world,
                          const Options &options,
                          uint32_t now) {
    const Font &font = Text.Game;
    const auto status = world.Talk.Status(now);
    const Position &center = gamestate.Map.Position;

    if (PlacedRevision != world.Talk.Revision() ||
        PlacedWidth != view.Width || PlacedHeight != view.Height ||
        PlacedStatus != status.has_value() || PlacedAt.X != center.X ||
        PlacedAt.Y != center.Y || PlacedAt.Z != center.Z) {
        PlaceTalk(view, gamestate, world, options, status.has_value());

        PlacedRevision = world.Talk.Revision();
        PlacedWidth = view.Width;
        PlacedHeight = view.Height;
        PlacedStatus = status.has_value();
        PlacedAt = center;
    }

    size_t index = 0;

    for (const Talkbox &talk : world.Talk.Boxes()) {
        if (!talk.Shown || index >= Placements.size()) {
            continue;
        }

        const Placement &placement = Placements[index++];
        const Pixel color = TalkColor(talk.Kind);
        int x = placement.X, y = placement.Y;

        /* Speech scrolls along with the map while the player walks. */
        if (!placement.PinnedX) {
            x -= options.NoStretching
                         ? frame.PlayerOffsetX
                         : view.Width * frame.PlayerOffsetX / MapFrame::Width;
        }

        if (!placement.PinnedY) {
            y -= options.NoStretching
                         ? frame.PlayerOffsetY
                         : view.Height * frame.PlayerOffsetY /
                                   MapFrame::Height;
        }

        for (size_t line = 0; line < talk.Lines.size(); line++) {
            font.Draw(canvas,
                      view.X + x,
                      view.Y + y +
                              static_cast<int>(line) * font.LinePitch(),
                      color,
                      talk.Lines[line],
                      placement.Alignment);
        }
    }

    if (status) {
        font.Draw(canvas,
                  view.X + view.Width / 2,
                  view.Y + view.Height - font.LineHeight(),
                  Pixel(255, 255, 255),
                  status->substr(0, font.TextFit(*status, view.Width)),
                  Align::Center);
    }
}

void MapOverlay::DrawTexts(Canvas &canvas,
                           const Rect &view,
                           const MapFrame &frame,
                           const Options &options) {
    if (!options.ShowTextualEffects) {
        return;
    }

    const Font &font = Text.Game;
    const Projection projection{view, options.NoStretching};

    for (const auto &mark : frame.Texts) {
        const bool visible = MapWindow::Contains(mark.Column, mark.Row) &&
                             !frame.Covered(mark.Column, mark.Row, mark.Level);

        if (!visible && mark.Level != frame.PlayerLevel) {
            continue;
        }

        const std::string text = std::to_string(mark.Value);
        const int textWidth = font.TextWidth(text);
        int x = mark.X, y = mark.Y;

        projection.Apply(x, y);

        Bound(x, textWidth / 2, view.Width - textWidth / 2);
        Bound(y, 0, view.Height - font.LineHeight());

        font.Draw(canvas,
                  view.X + x,
                  view.Y + y,
                  Pixel::ProtocolColor(mark.Color),
                  text,
                  Align::Center);
    }
}

} // namespace Client
} // namespace trc
