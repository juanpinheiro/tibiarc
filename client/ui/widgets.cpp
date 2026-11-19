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

#include "ui/widgets.hpp"

#include "scene/animation.hpp"
#include "scene/parts.hpp"

#include <algorithm>

namespace trc {
namespace Client {

namespace {

/* Sprites are squares of this many pixels. */
constexpr int SpriteSize = 32;

/* Objects in the interface change their picture every this many ticks. */
constexpr int TicksPerPhase = 5;

/* Which way the creatures of the interface face: south. */
constexpr int OutfitDirection = 2;

/* Skins of the scrollbar: its arrows, its track, and the thumb, which is
 * two caps with a stretch of body in between. */
constexpr int ScrollUpSkin = 157;
constexpr int ScrollDownSkin = 159;
constexpr int ScrollTrackSkin = 153;
constexpr int ScrollThumbTopSkin = 154;
constexpr int ScrollThumbBodySkin = 155;
constexpr int ScrollThumbBottomSkin = 156;
constexpr int ScrollTrackMinimumHeight = 14;

/* The caption of a button that is held down moves this far right and down. */
constexpr int PressedShift = 1;

const TextStyle CaptionStyle{3, Pixel(255, 255, 255), Align::Center};
const TextStyle CountStyle{2, Pixel(192, 192, 192), Align::Right};
const Pixel GaugeFrameColor(0, 0, 0);

} // namespace

const ButtonSkins ButtonSkins::Regular{12, 13};
const ButtonSkins ButtonSkins::Narrow{14, 15};

Widgets::Widgets(const Version &version, const Fonts &fonts, const Skin &skin)
    : Text(fonts),
      Pieces(skin),
      Data(version),
      Scratch(SpriteSize, SpriteSize) {
}

void Widgets::Write(Canvas &canvas,
                    int x,
                    int y,
                    const TextStyle &style,
                    std::string_view text) const {
    Text[style.Font].Draw(canvas, x, y, style.Color, text, style.Alignment);
}

void Widgets::Label(Canvas &canvas,
                    const Rect &area,
                    const TextStyle &style,
                    std::string_view text) const {
    const Font &font = Text[style.Font];
    const ClipScope clip(canvas, area);
    int x = area.X;

    if (style.Alignment == Align::Center) {
        x += area.Width / 2;
    } else if (style.Alignment == Align::Right) {
        x += area.Width;
    }

    Write(canvas,
          x,
          area.Y + (area.Height - font.LineHeight()) / 2,
          style,
          text);
}

std::string Widgets::Abbreviate(int font,
                                std::string_view text,
                                int maxWidth) const {
    static constexpr size_t Dots = 3;
    const size_t fit = Text[font].TextFit(text, maxWidth);

    if (fit >= text.size()) {
        return std::string(text);
    }

    std::string result(text.substr(0, fit < Dots ? 0 : fit - Dots));

    result.append(std::min(text.size(), Dots), '.');

    return result;
}

void Widgets::Button(Canvas &canvas,
                     const ButtonSkins &skins,
                     int x,
                     int y,
                     bool down) const {
    Pieces.Draw(canvas, down ? skins.Down : skins.Up, x, y);
}

void Widgets::TextButton(Canvas &canvas,
                         const ButtonSkins &skins,
                         int x,
                         int y,
                         std::string_view caption,
                         bool down) const {
    const int shift = down ? PressedShift : 0;
    const int slack = Skin::Height(skins.Up) -
                      Text[CaptionStyle.Font].LineHeight();

    Button(canvas, skins, x, y, down);
    Write(canvas,
          x + Skin::Width(skins.Up) / 2 + shift,
          y + slack / 2 + shift,
          CaptionStyle,
          caption);
}

void Widgets::Scrollbar(Canvas &canvas,
                        int x,
                        int y,
                        int height,
                        int total,
                        int page,
                        int position) const {
    const int arrow = Skin::Height(ScrollUpSkin);
    const int track = std::max(height - arrow * 2, ScrollTrackMinimumHeight);
    const int caps = Skin::Height(ScrollThumbTopSkin) +
                     Skin::Height(ScrollThumbBottomSkin);

    Pieces.Draw(canvas, ScrollUpSkin, x, y);
    Pieces.Tile(canvas,
                ScrollTrackSkin,
                Rect{x, y + arrow, ScrollbarWidth, track});
    Pieces.Draw(canvas, ScrollDownSkin, x, y + arrow + track);

    if (total <= page) {
        return;
    }

    /* The thumb is as large a share of the track as the page is of the
     * whole, but never smaller than its two caps. */
    const float share = static_cast<float>(page) / static_cast<float>(total);
    const int thumb = std::max(
            caps,
            static_cast<int>(static_cast<float>(track) * share));
    const int top = y + arrow + (track - thumb) * position / (total - page);

    Pieces.Draw(canvas, ScrollThumbTopSkin, x, top);
    Pieces.Tile(canvas,
                ScrollThumbBodySkin,
                Rect{x,
                     top + Skin::Height(ScrollThumbTopSkin),
                     ScrollbarWidth,
                     thumb - caps});
    Pieces.Draw(canvas,
                ScrollThumbBottomSkin,
                x,
                top + thumb - Skin::Height(ScrollThumbBottomSkin));
}

void Widgets::Gauge(Canvas &canvas,
                    const Rect &frame,
                    int percent,
                    const Pixel &color) const {
    canvas.Frame(frame, 1, GaugeFrameColor);
    canvas.Fill(Rect{frame.X + 1,
                     frame.Y + 1,
                     (frame.Width - 2) * percent / 100,
                     frame.Height - 2},
                color);
}

void Widgets::DrawPart(Canvas &canvas,
                       const Sprite &sprite,
                       int x,
                       int y,
                       int size,
                       int scale,
                       int column,
                       int row) {
    const Rect target{x + size - (column + 1) * scale,
                      y + size - (row + 1) * scale,
                      scale,
                      scale};

    if (scale == SpriteSize) {
        canvas.Draw(sprite, target.X, target.Y);
        return;
    }

    Scratch.Wipe();
    Scratch.Draw(sprite, 0, 0);
    canvas.StretchNearest(Scratch, Scratch.Bounds(), target);
}

void Widgets::ObjectIcon(Canvas &canvas,
                         int x,
                         int y,
                         int size,
                         const Object &object,
                         uint32_t now) {
    if (object.Id == 0) {
        return;
    }

    const EntityType &type = Data.GetItem(object.Id);
    const auto &group = DefaultGroup(type);
    const auto &properties = type.Properties;
    const int scale = size * SpriteSize / std::max<int>(group.RenderSize, 1);
    int patternX = 0, patternY = 0;
    int phase;

    if (properties.Stackable) {
        const int picture = StackPicture(object.ExtraByte);

        patternX = picture % 4;
        patternY = picture / 4;
    }

    if (properties.LiquidPool || properties.LiquidContainer) {
        const int liquid = Data.TranslateFluidColor(object.ExtraByte);

        patternX = liquid % 4;
        patternY = liquid / 4;
    }

    if (Data.Features.AnimationPhases) {
        phase = object.Animation;
    } else {
        phase = static_cast<int>(Clock::Ticks(now) / TicksPerPhase);
    }

    for (int layer = 0; layer < group.LayerCount; layer++) {
        for (int column = 0; column < group.SizeX; column++) {
            for (int row = 0; row < group.SizeY; row++) {
                if (const Sprite *sprite = PartSprite(group,
                                                      column,
                                                      row,
                                                      layer,
                                                      patternX,
                                                      patternY,
                                                      0,
                                                      phase)) {
                    DrawPart(canvas, *sprite, x, y, size, scale, column, row);
                }
            }
        }
    }

    if (properties.Stackable && object.ExtraByte > 1) {
        const std::string count = std::to_string(object.ExtraByte);

        /* The count sits in the bottom-right corner, a pixel in from both
         * edges whatever the height of its digits. */
        Write(canvas,
              x + size - 1,
              y + size - 1 - Text[CountStyle.Font].TextHeight(count),
              CountStyle,
              count);
    }
}

void Widgets::OutfitIcon(Canvas &canvas,
                         int x,
                         int y,
                         int size,
                         const Appearance &outfit,
                         uint32_t now) {
    if (outfit.Id == 0) {
        /* A creature that looks like an object. */
        ObjectIcon(canvas, x, y, size, Object(outfit.Item.Id), now);
        return;
    }

    const auto &group = DefaultGroup(Data.GetOutfit(outfit.Id));
    const int scale = size * SpriteSize / std::max<int>(group.RenderSize, 1);

    for (int column = 0; column < group.SizeX; column++) {
        for (int row = 0; row < group.SizeY; row++) {
            const auto part = [&](int layer) {
                return PartSprite(group,
                                  column,
                                  row,
                                  layer,
                                  OutfitDirection,
                                  0,
                                  0,
                                  0);
            };
            const Sprite *sprite = part(0);

            if (sprite == nullptr) {
                continue;
            }

            if (group.LayerCount >= 2) {
                if (const Sprite *mask = part(1)) {
                    sprite = &Colorizer.Get(*sprite, *mask, outfit);
                }
            }

            DrawPart(canvas, *sprite, x, y, size, scale, column, row);
        }
    }
}

} // namespace Client
} // namespace trc
