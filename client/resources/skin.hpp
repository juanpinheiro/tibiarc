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

#ifndef __TRC_CLIENT_ASSETS_SKIN_HPP__
#define __TRC_CLIENT_ASSETS_SKIN_HPP__

#include "graphics/canvas.hpp"

namespace trc {
namespace Client {

/* The interface is assembled from small pieces ("skins") cut out of one
 * picture. Skins are never scaled: an area larger than a skin is covered by
 * repeating it. */
class Skin {
public:
    static constexpr int Count = 262;

    /* Frequently used pieces. */
    enum Id : int {
        None = 0,
        Background = 7,
        LineBottom = 8,
        LineTop = 9,
        LineRight = 10,
        LineLeft = 11,
    };

    explicit Skin(const Canvas &atlas) : Atlas(atlas) {
    }

    static const Rect &Area(int id);

    static int Width(int id) {
        return Area(id).Width;
    }

    static int Height(int id) {
        return Area(id).Height;
    }

    /* Draws a skin once, at its own size. */
    void Draw(Canvas &canvas, int id, int x, int y) const;

    /* Covers `area` with copies of a skin; the first column and row start
     * (offsetX, offsetY) pixels into it. */
    void Tile(Canvas &canvas,
              int id,
              const Rect &area,
              int offsetX = 0,
              int offsetY = 0) const;

    /* Copies an arbitrary part of the picture. */
    void DrawRegion(Canvas &canvas, const Rect &source, int x, int y) const;

private:
    const Canvas &Atlas;
};

} // namespace Client
} // namespace trc

#endif
