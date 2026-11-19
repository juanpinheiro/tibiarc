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

#include "scene/light.hpp"

namespace trc {
namespace Client {

void LightMap::Dim() {
    for (auto &field : Fields) {
        field = field.Scale(4, 5);
    }
}

void LightMap::Shade(Canvas &canvas, int scrollX, int scrollY) const {
    auto corner = [this](int column, int row) -> const Color & {
        return At(std::clamp(column, 0, Columns - 1),
                  std::clamp(row, 0, Rows - 1));
    };

    for (int row = 0; row <= Rows; row++) {
        for (int column = 0; column <= Columns; column++) {
            const Rect quad = Rect{column * 32 - scrollX - 16,
                                   row * 32 - scrollY - 16,
                                   32,
                                   32};
            const Rect area = quad.Intersect(canvas.Clip());

            if (area.Empty()) {
                continue;
            }

            const Color &topLeft = corner(column - 1, row - 1);
            const Color &topRight = corner(column, row - 1);
            const Color &bottomLeft = corner(column - 1, row);
            const Color &bottomRight = corner(column, row);

            for (int y = area.Y; y < area.Bottom(); y++) {
                /* Sample at pixel centres, in sixty-fourths of a field. */
                const int wy = (y - quad.Y) * 2 + 1;
                Pixel *pixels = &canvas.GetPixel(area.X, y);

                auto vertical = [wy](int top, int bottom) {
                    return top * (64 - wy) + bottom * wy;
                };

                const int leftR = vertical(topLeft.Red, bottomLeft.Red);
                const int leftG = vertical(topLeft.Green, bottomLeft.Green);
                const int leftB = vertical(topLeft.Blue, bottomLeft.Blue);
                const int rightR = vertical(topRight.Red, bottomRight.Red);
                const int rightG = vertical(topRight.Green, bottomRight.Green);
                const int rightB = vertical(topRight.Blue, bottomRight.Blue);

                for (int x = area.X; x < area.Right(); x++) {
                    const int wx = (x - quad.X) * 2 + 1;
                    Pixel &pixel = pixels[x - area.X];

                    /* Light is in 0..255 scaled by 64 * 64. */
                    const int red = leftR * (64 - wx) + rightR * wx;
                    const int green = leftG * (64 - wx) + rightG * wx;
                    const int blue = leftB * (64 - wx) + rightB * wx;

                    pixel.Red = static_cast<uint8_t>(
                            (pixel.Red * (red >> 4) + 32640) / 65280);
                    pixel.Green = static_cast<uint8_t>(
                            (pixel.Green * (green >> 4) + 32640) / 65280);
                    pixel.Blue = static_cast<uint8_t>(
                            (pixel.Blue * (blue >> 4) + 32640) / 65280);
                }
            }
        }
    }
}

} // namespace Client
} // namespace trc
