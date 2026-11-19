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

#ifndef __TRC_CLIENT_SCENE_LIGHT_HPP__
#define __TRC_CLIENT_SCENE_LIGHT_HPP__

#include "graphics/canvas.hpp"

#include <array>

namespace trc {
namespace Client {

/* Light is tracked per field of the visible map and then smoothly blended
 * across the picture. */
class LightMap {
public:
    static constexpr int Columns = 18;
    static constexpr int Rows = 14;

    struct Color {
        int Red = 0, Green = 0, Blue = 0;

        static Color FromPalette(int index) {
            return Color{(index / 36) * 51,
                         ((index / 6) % 6) * 51,
                         (index % 6) * 51};
        }

        Color Scale(int numerator, int denominator) const {
            return Color{Red * numerator / denominator,
                         Green * numerator / denominator,
                         Blue * numerator / denominator};
        }

        Color Max(const Color &other) const {
            return Color{std::max(Red, other.Red),
                         std::max(Green, other.Green),
                         std::max(Blue, other.Blue)};
        }
    };

    Color &At(int column, int row) {
        return Fields[column * Rows + row];
    }

    const Color &At(int column, int row) const {
        return Fields[column * Rows + row];
    }

    /* Dims every field to four fifths, as happens for the light of the
     * floors below the player. */
    void Dim();

    /* Multiplies the map picture with the light. Field (column, row) is
     * centred on pixel (column * 32 - scrollX + 16, row * 32 - scrollY + 16)
     * and the light is interpolated between the field centres. */
    void Shade(Canvas &canvas, int scrollX, int scrollY) const;

private:
    std::array<Color, Columns * Rows> Fields;
};

} // namespace Client
} // namespace trc

#endif
