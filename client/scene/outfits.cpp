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

#include "scene/outfits.hpp"

#include "graphics/canvas.hpp"

#include <cmath>

namespace trc {
namespace Client {

/* The outfit palette is a 19 by 7 grid: a column of greys followed by 18
 * hues, each row a fixed saturation and brightness. */
Pixel OutfitColorizer::PaletteColor(int index) {
    static constexpr int Columns = 19, Rows = 7;

    if (index < 0 || index >= Columns * Rows) {
        index = 0;
    }

    const int column = index % Columns;
    const int row = index / Columns;

    double hue = 0.0, saturation = 0.0, value = 1.0 - row * (1.0 / 7.0);

    if (column != 0) {
        static constexpr double Saturations[Rows] =
                {0.25, 0.25, 0.5, 0.667, 1.0, 1.0, 1.0};
        static constexpr double Values[Rows] =
                {1.0, 0.75, 0.75, 0.75, 1.0, 0.75, 0.5};

        hue = column * (1.0 / 18.0);
        saturation = Saturations[row];
        value = Values[row];
    }

    double red = value, green = value, blue = value;

    if (saturation > 0.0) {
        const double sector = (hue >= 1.0 ? 0.0 : hue) * 6.0;
        const int which = static_cast<int>(std::floor(sector));
        const double fraction = sector - which;
        const double p = value * (1.0 - saturation);
        const double q = value * (1.0 - saturation * fraction);
        const double t = value * (1.0 - saturation * (1.0 - fraction));

        switch (which) {
        case 0:
            red = value, green = t, blue = p;
            break;
        case 1:
            red = q, green = value, blue = p;
            break;
        case 2:
            red = p, green = value, blue = t;
            break;
        case 3:
            red = p, green = q, blue = value;
            break;
        case 4:
            red = t, green = p, blue = value;
            break;
        default:
            red = value, green = p, blue = q;
            break;
        }
    }

    return Pixel(static_cast<uint8_t>(red * 255.0),
                 static_cast<uint8_t>(green * 255.0),
                 static_cast<uint8_t>(blue * 255.0));
}

const Sprite &OutfitColorizer::Get(const Sprite &base,
                                   const Sprite &mask,
                                   const Appearance &colors) {
    const Key key{&base,
                  &mask,
                  (static_cast<uint32_t>(colors.HeadColor) << 24) |
                          (static_cast<uint32_t>(colors.PrimaryColor) << 16) |
                          (static_cast<uint32_t>(colors.SecondaryColor) << 8) |
                          colors.DetailColor};

    if (auto it = Cache.find(key); it != Cache.end()) {
        return it->second;
    }

    if (Cache.size() >= MaxEntries) {
        Cache.clear();
    }

    const int width = std::max(base.Width, 1);
    const int height = std::max(base.Height, 1);

    Canvas picture(width, height), areas(width, height);

    picture.Wipe();
    picture.Draw(base, 0, 0);
    areas.Wipe();
    areas.Draw(mask, 0, 0);

    const Pixel palette[4] = {PaletteColor(colors.HeadColor),
                              PaletteColor(colors.PrimaryColor),
                              PaletteColor(colors.SecondaryColor),
                              PaletteColor(colors.DetailColor)};

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const Pixel &area = areas.GetPixel(x, y);
            int part = -1;

            if (area.IsTransparent()) {
                continue;
            } else if (area.Red == 0xFF && area.Green == 0xFF &&
                       area.Blue == 0) {
                part = 0;
            } else if (area.Red == 0xFF && area.Green == 0 && area.Blue == 0) {
                part = 1;
            } else if (area.Red == 0 && area.Green == 0xFF && area.Blue == 0) {
                part = 2;
            } else if (area.Red == 0 && area.Green == 0 && area.Blue == 0xFF) {
                part = 3;
            }

            if (part >= 0) {
                Pixel &pixel = picture.GetPixel(x, y);
                const int grey = pixel.IsTransparent()
                                         ? 0
                                         : (pixel.Red * 30 + pixel.Green * 59 +
                                            pixel.Blue * 11) /
                                                   100;

                pixel = Pixel(palette[part].Red * grey / 256,
                              palette[part].Green * grey / 256,
                              palette[part].Blue * grey / 256);
            }
        }
    }

    auto [it, _] = Cache.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(key),
                                 std::forward_as_tuple(picture,
                                                       0,
                                                       0,
                                                       width,
                                                       height));

    return it->second;
}

} // namespace Client
} // namespace trc
