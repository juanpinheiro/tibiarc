/*
 * Copyright 2011-2016 "Silver Squirrel Software Handelsbolag"
 * Copyright 2023-2026 "John Högberg"
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

#ifndef __TRC_CANVAS_HPP__
#define __TRC_CANVAS_HPP__

#include <algorithm>
#include <cstdint>
#include <memory>

#include "common/pixel.hpp"
#include "graphics/sprites.hpp"

namespace trc {

struct Rect {
    int X = 0;
    int Y = 0;
    int Width = 0;
    int Height = 0;

    constexpr int Right() const {
        return X + Width;
    }

    constexpr int Bottom() const {
        return Y + Height;
    }

    constexpr bool Empty() const {
        return Width <= 0 || Height <= 0;
    }

    constexpr bool Contains(int x, int y) const {
        return x >= X && y >= Y && x < Right() && y < Bottom();
    }

    constexpr Rect Intersect(const Rect &other) const {
        const int left = std::max(X, other.X);
        const int top = std::max(Y, other.Y);
        const int right = std::min(Right(), other.Right());
        const int bottom = std::min(Bottom(), other.Bottom());

        return Rect{left,
                    top,
                    std::max(0, right - left),
                    std::max(0, bottom - top)};
    }

    constexpr Rect Translate(int dx, int dy) const {
        return Rect{X + dx, Y + dy, Width, Height};
    }

    constexpr bool operator==(const Rect &) const = default;
};

/* A 32-bit RGBA pixel surface with a clip rectangle. Every drawing operation
 * takes absolute coordinates and is confined to the current clip, so callers
 * never have to care about going out of bounds. */
class Canvas {
    std::unique_ptr<uint8_t[]> Storage;
    Rect ClipRect;

public:
    int Width;
    int Height;
    int Stride;
    uint8_t *Buffer;

    Canvas(int width, int height);

    /* Wraps a buffer owned by someone else. */
    Canvas(int width, int height, int stride, uint8_t *buffer);

    Canvas(const Canvas &other) = delete;
    Canvas(Canvas &&other) = default;

    Rect Bounds() const {
        return Rect{0, 0, Width, Height};
    }

    const Rect &Clip() const {
        return ClipRect;
    }

    /* Sets the clip rectangle, returning the previous one. The rectangle is
     * always confined to the canvas itself. */
    Rect SetClip(const Rect &clip) {
        Rect previous = ClipRect;
        ClipRect = clip.Intersect(Bounds());
        return previous;
    }

    Pixel &GetPixel(int x, int y) {
        return *reinterpret_cast<Pixel *>(
                &Buffer[x * sizeof(Pixel) + y * Stride]);
    }

    const Pixel &GetPixel(int x, int y) const {
        return *reinterpret_cast<const Pixel *>(
                &Buffer[x * sizeof(Pixel) + y * Stride]);
    }

    /* Makes the whole canvas transparent, ignoring the clip. */
    void Wipe();

    void Fill(const Rect &area, const Pixel &color);

    /* Outlines `area` with a border of the given thickness, inside of it. */
    void Frame(const Rect &area, int thickness, const Pixel &color);

    void Draw(const Sprite &sprite, int x, int y);

    /* Draws a font glyph, whose pixels are an intensity that is multiplied
     * with the given colour. */
    void DrawGlyph(const Sprite &glyph, const Pixel &color, int x, int y);

    /* Copies the opaque pixels of `source` in `from` to (x, y). */
    void Copy(const Canvas &from, const Rect &source, int x, int y);

    /* Covers `area` with repeated copies of `source` in `from`. The pattern
     * is anchored at (originX, originY) within the source rectangle, so
     * adjacent areas tile seamlessly. */
    void Tile(const Canvas &from,
              const Rect &source,
              const Rect &area,
              int originX = 0,
              int originY = 0);

    /* Scales `source` in `from` onto `target` with bilinear filtering,
     * sampling at pixel centres and clamping at the source's edges. */
    void Stretch(const Canvas &from, const Rect &source, const Rect &target);

    /* As Stretch, but picks the nearest source pixel and skips transparent
     * ones. */
    void StretchNearest(const Canvas &from,
                        const Rect &source,
                        const Rect &target);
};

/* Scoped clip: narrows the canvas' clip rectangle and restores it on exit. */
class ClipScope {
    Canvas &Target;
    Rect Previous;

public:
    ClipScope(Canvas &canvas, const Rect &clip)
        : Target(canvas),
          Previous(canvas.SetClip(clip.Intersect(canvas.Clip()))) {
    }

    ~ClipScope() {
        Target.SetClip(Previous);
    }

    ClipScope(const ClipScope &) = delete;
};

} // namespace trc

#endif /* __TRC_CANVAS_HPP__ */
