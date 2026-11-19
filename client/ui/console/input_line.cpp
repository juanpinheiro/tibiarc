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

#include "ui/console/input_line.hpp"

namespace trc {
namespace Client {

static const Pixel FieldColor(54, 54, 54);
static const Pixel CaretColor(255, 255, 255);

/* Distance from the field's corner to where its text starts. */
static constexpr int TextMargin = 3;

/* The caret is shown and hidden for this long in turn. */
static constexpr uint32_t CaretBlinkTime = 500;

static void DrawBorder(Canvas &canvas, const Skin &skin, const Rect &area) {
    skin.Tile(canvas, Skin::LineTop, Rect{area.X, area.Y, area.Width, 1});
    skin.Tile(canvas,
              Skin::LineLeft,
              Rect{area.X, area.Y + 1, 1, area.Height - 1});
    skin.Tile(canvas,
              Skin::LineRight,
              Rect{area.Right() - 1, area.Y + 1, 1, area.Height - 1});
    skin.Tile(canvas,
              Skin::LineBottom,
              Rect{area.X + 1, area.Bottom() - 1, area.Width - 2, 1});
}

void DrawInputLine(Canvas &canvas,
                   const Fonts &fonts,
                   const Skin &skin,
                   const Rect &area,
                   uint32_t now) {
    canvas.Fill(area, FieldColor);
    DrawBorder(canvas, skin, area);

    /* The caret sits where the next character would go and is as tall as
     * the tallest character of the font. */
    if ((now / CaretBlinkTime) % 2 == 0) {
        canvas.Fill(Rect{area.X + TextMargin,
                         area.Y + TextMargin,
                         1,
                         fonts.Interface.LineHeight()},
                    CaretColor);
    }
}

} // namespace Client
} // namespace trc
