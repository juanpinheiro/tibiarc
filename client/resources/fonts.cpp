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

#include "resources/fonts.hpp"

namespace trc {
namespace Client {

Font::Font(const Canvas &picture,
           int cellWidth,
           int cellHeight,
           int spacing,
           int linePitch)
    : Spacing(spacing), Pitch(linePitch), Tallest(0) {
    for (int index = 0; index < GlyphCount; index++) {
        const int cellX = cellWidth * (index % GlyphsPerRow);
        const int cellY = cellHeight * (index / GlyphsPerRow);
        int width = cellWidth, height = 1;

        if (cellX + cellWidth > picture.Width ||
            cellY + cellHeight > picture.Height) {
            Widths[index] = static_cast<uint8_t>(width);
            Heights[index] = static_cast<uint8_t>(height);
            continue;
        }

        auto inked = [&](int column, int row) {
            return !picture.GetPixel(cellX + column, cellY + row)
                            .IsTransparent();
        };

        /* The first column and row are deliberately left out of the
         * search: an empty cell ends up as wide as the cell itself. */
        for (int column = cellWidth - 1; column > 0; column--) {
            bool found = false;

            for (int row = 0; row < cellHeight && !found; row++) {
                found = inked(column, row);
            }

            if (found) {
                width = column + 1;
                break;
            }
        }

        for (int row = cellHeight - 1; row > 0; row--) {
            bool found = false;

            for (int column = 0; column < cellWidth && !found; column++) {
                found = inked(column, row);
            }

            if (found) {
                height = row + 1;
                break;
            }
        }

        Widths[index] = static_cast<uint8_t>(width);
        Heights[index] = static_cast<uint8_t>(height);
        Tallest = std::max(Tallest, height);

        Glyphs[index] = Sprite(picture, cellX, cellY, cellWidth, cellHeight);
    }
}

int Font::GlyphIndex(char character) {
    const auto code = static_cast<signed char>(character);

    if (code >= '!') {
        return code - 0x20;
    } else if (code < 0) {
        return code + 0xE0;
    } else if (code == '\n' || code == ' ') {
        return -1;
    }

    return 0x5F;
}

int Font::Advance(char character) const {
    const int index = GlyphIndex(character);

    /* A space is as wide as a comma. */
    return Widths[index < 0 ? SpaceGlyph : index] + Spacing;
}

int Font::TextWidth(std::string_view text) const {
    int width = 0;

    for (char character : text) {
        width += Advance(character);
    }

    return text.empty() ? 0 : width - Spacing;
}

int Font::TextHeight(std::string_view text) const {
    int height = 0;

    for (char character : text) {
        if (const int index = GlyphIndex(character); index >= 0) {
            height = std::max<int>(height, Heights[index]);
        }
    }

    return height;
}

size_t Font::TextFit(std::string_view text, int maxWidth) const {
    int width = -Spacing;

    for (size_t i = 0; i < text.size(); i++) {
        width += Advance(text[i]);

        if (width > maxWidth) {
            return i;
        }
    }

    return text.size();
}

void Font::Draw(Canvas &canvas,
                int x,
                int y,
                const Pixel &color,
                std::string_view text,
                Align align) const {
    if (align == Align::Center) {
        x -= TextWidth(text) / 2;
    } else if (align == Align::Right) {
        x -= TextWidth(text);
    }

    for (char character : text) {
        if (const int index = GlyphIndex(character); index >= 0) {
            canvas.DrawGlyph(Glyphs[index], color, x, y);
        }

        x += Advance(character);
    }
}

Fonts::Fonts(const PictureFile &pictures)
    : Tooltip(pictures.Get(PictureIndex::FontUnbordered), 8, 16, 1, 12),
      Game(pictures.Get(PictureIndex::FontGame), 16, 16, -1, 15),
      Caption(pictures.Get(PictureIndex::FontInterfaceSmall), 8, 8, 0, 10),
      Interface(pictures.Get(PictureIndex::FontInterfaceLarge), 8, 16, 1, 14) {
}

const Font &Fonts::operator[](int number) const {
    switch (number) {
    case 1:
        return Tooltip;
    case 2:
        return Game;
    case 4:
        return Interface;
    default:
        return Caption;
    }
}

} // namespace Client
} // namespace trc
