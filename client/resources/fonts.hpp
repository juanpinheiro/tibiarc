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

#ifndef __TRC_CLIENT_ASSETS_FONTS_HPP__
#define __TRC_CLIENT_ASSETS_FONTS_HPP__

#include "assets/pictures.hpp"
#include "graphics/canvas.hpp"

#include <array>
#include <string_view>

namespace trc {
namespace Client {

enum class Align { Left = 0, Center = 1, Right = 2 };

/* A bitmap font: a grid of glyph cells in a picture, with the width of each
 * glyph measured from its pixels. Text is always a single line of Latin-1;
 * laying out several lines is up to the caller. */
class Font {
public:
    static constexpr int GlyphCount = 224;
    static constexpr int GlyphsPerRow = 32;

    Font(const Canvas &picture,
         int cellWidth,
         int cellHeight,
         int spacing,
         int linePitch);

    Font(const Font &) = delete;

    /* Distance between the tops of consecutive lines. */
    int LinePitch() const {
        return Pitch;
    }

    /* Height of the tallest glyph, used to centre text vertically. */
    int LineHeight() const {
        return Tallest;
    }

    int TextWidth(std::string_view text) const;

    /* Height of the tallest glyph in the given text. */
    int TextHeight(std::string_view text) const;

    /* The number of leading characters that fit within `maxWidth`. */
    size_t TextFit(std::string_view text, int maxWidth) const;

    /* Draws text with its top at `y`, anchored horizontally at `x`
     * according to the alignment. */
    void Draw(Canvas &canvas,
              int x,
              int y,
              const Pixel &color,
              std::string_view text,
              Align align = Align::Left) const;

private:
    static constexpr int SpaceGlyph = ',' - ' ';

    static int GlyphIndex(char character);

    int Advance(char character) const;

    int Spacing;
    int Pitch;
    int Tallest;

    std::array<uint8_t, GlyphCount> Widths;
    std::array<uint8_t, GlyphCount> Heights;
    std::array<Sprite, GlyphCount> Glyphs;
};

struct Fonts {
    /* Small text with generous spacing: tooltips. */
    Font Tooltip;
    /* Outlined text drawn over the map: names, speech, damage. */
    Font Game;
    /* Tiny captions on buttons and tabs. */
    Font Caption;
    /* The regular interface font: console, lists, labels. */
    Font Interface;

    explicit Fonts(const PictureFile &pictures);

    /* Fonts by the number the layout tables use, 1 to 4. */
    const Font &operator[](int number) const;
};

} // namespace Client
} // namespace trc

#endif
