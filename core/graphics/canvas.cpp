/*
 * Copyright 2011-2016 "Silver Squirrel Software Handelsbolag"
 * Copyright 2023-2026 "John Högberg"
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

#include "graphics/canvas.hpp"

#include "common/utils.hpp"

#include <cstring>

namespace trc {

static_assert(sizeof(Pixel) == 4 && alignof(Pixel) == 1,
              "Pixel struct must be byte-aligned");

Canvas::Canvas(int width, int height)
    : Storage(std::make_unique_for_overwrite<uint8_t[]>(
              static_cast<size_t>(std::max(width, 1)) * std::max(height, 1) *
              sizeof(Pixel))),
      ClipRect{0, 0, width, height},
      Width(width),
      Height(height),
      Stride(width * static_cast<int>(sizeof(Pixel))),
      Buffer(Storage.get()) {
    AbortUnless(width >= 0 && height >= 0);
}

Canvas::Canvas(int width, int height, int stride, uint8_t *buffer)
    : ClipRect{0, 0, width, height},
      Width(width),
      Height(height),
      Stride(stride),
      Buffer(buffer) {
}

void Canvas::Wipe() {
    for (int y = 0; y < Height; y++) {
        memset(&GetPixel(0, y), 0, Width * sizeof(Pixel));
    }
}

void Canvas::Fill(const Rect &area, const Pixel &color) {
    const Rect clipped = area.Intersect(ClipRect);

    if (clipped.Empty()) {
        return;
    }

    Pixel *first = &GetPixel(clipped.X, clipped.Y);
    std::fill_n(first, clipped.Width, color);

    for (int y = clipped.Y + 1; y < clipped.Bottom(); y++) {
        memcpy(&GetPixel(clipped.X, y), first, clipped.Width * sizeof(Pixel));
    }
}

void Canvas::Frame(const Rect &area, int thickness, const Pixel &color) {
    Fill(Rect{area.X, area.Y, area.Width, thickness}, color);
    Fill(Rect{area.X, area.Bottom() - thickness, area.Width, thickness},
         color);
    Fill(Rect{area.X,
              area.Y + thickness,
              thickness,
              area.Height - thickness * 2},
         color);
    Fill(Rect{area.Right() - thickness,
              area.Y + thickness,
              thickness,
              area.Height - thickness * 2},
         color);
}

namespace {

/* Walks the opaque runs of a sprite, calling `emit(targetX, targetY, pixels,
 * count)` for every horizontal span that survives clipping. */
template <typename Emit>
void ForEachSpan(const Sprite &sprite,
                 const Rect &clip,
                 int x,
                 int y,
                 Emit &&emit) {
    const Rect area =
            Rect{x, y, sprite.Width, sprite.Height}.Intersect(clip);

    if (area.Empty() || sprite.Buffer == nullptr) {
        return;
    }

    const uint8_t *data = sprite.Buffer;
    size_t byteIdx = 0, pixelIdx = 0;

    while (byteIdx + 4 <= sprite.Size) {
        pixelIdx += static_cast<size_t>(data[byteIdx]) |
                    (static_cast<size_t>(data[byteIdx + 1]) << 8);
        size_t run = static_cast<size_t>(data[byteIdx + 2]) |
                     (static_cast<size_t>(data[byteIdx + 3]) << 8);
        byteIdx += 4;

        while (run > 0) {
            const int fromX = static_cast<int>(pixelIdx % sprite.Width);
            const int fromY = static_cast<int>(pixelIdx / sprite.Width);
            const int span = static_cast<int>(
                    std::min<size_t>(run, sprite.Width - fromX));
            const int targetY = y + fromY;

            if (targetY >= area.Bottom()) {
                return;
            }

            if (targetY >= area.Y) {
                const int left = std::max(x + fromX, area.X);
                const int right = std::min(x + fromX + span, area.Right());

                if (left < right) {
                    const auto *pixels = reinterpret_cast<const Pixel *>(
                            &data[byteIdx + (left - (x + fromX)) *
                                                    sizeof(Pixel)]);
                    emit(left, targetY, pixels, right - left);
                }
            }

            byteIdx += span * sizeof(Pixel);
            pixelIdx += span;
            run -= span;
        }
    }
}

} // namespace

void Canvas::Draw(const Sprite &sprite, int x, int y) {
    ForEachSpan(sprite,
                ClipRect,
                x,
                y,
                [this](int targetX,
                       int targetY,
                       const Pixel *pixels,
                       int count) {
                    memcpy(&GetPixel(targetX, targetY),
                           pixels,
                           count * sizeof(Pixel));
                });
}

void Canvas::DrawGlyph(const Sprite &glyph,
                       const Pixel &color,
                       int x,
                       int y) {
    ForEachSpan(glyph,
                ClipRect,
                x,
                y,
                [this, &color](int targetX,
                               int targetY,
                               const Pixel *pixels,
                               int count) {
                    Pixel *target = &GetPixel(targetX, targetY);

                    for (int i = 0; i < count; i++) {
                        target[i].Red = (pixels[i].Red * color.Red) / 255;
                        target[i].Green =
                                (pixels[i].Green * color.Green) / 255;
                        target[i].Blue = (pixels[i].Blue * color.Blue) / 255;
                        target[i].Alpha = 0xFF;
                    }
                });
}

void Canvas::Copy(const Canvas &from, const Rect &source, int x, int y) {
    const Rect valid = source.Intersect(from.Bounds());
    const Rect area = Rect{x + (valid.X - source.X),
                           y + (valid.Y - source.Y),
                           valid.Width,
                           valid.Height}
                              .Intersect(ClipRect);

    for (int targetY = area.Y; targetY < area.Bottom(); targetY++) {
        const Pixel *in =
                &from.GetPixel(source.X + (area.X - x), source.Y + (targetY - y));
        Pixel *out = &GetPixel(area.X, targetY);

        for (int i = 0; i < area.Width; i++) {
            if (!in[i].IsTransparent()) {
                out[i] = in[i];
            }
        }
    }
}

void Canvas::Tile(const Canvas &from,
                  const Rect &source,
                  const Rect &area,
                  int originX,
                  int originY) {
    if (source.Empty()) {
        return;
    }

    ClipScope scope(*this, area);

    originX %= source.Width;
    originY %= source.Height;

    for (int y = area.Y - originY; y < area.Bottom(); y += source.Height) {
        for (int x = area.X - originX; x < area.Right(); x += source.Width) {
            Copy(from, source, x, y);
        }
    }
}

void Canvas::Stretch(const Canvas &from,
                     const Rect &source,
                     const Rect &target) {
    const Rect area = target.Intersect(ClipRect);

    if (area.Empty() || source.Empty()) {
        return;
    }

    if (source.Width == target.Width && source.Height == target.Height) {
        for (int y = area.Y; y < area.Bottom(); y++) {
            memcpy(&GetPixel(area.X, y),
                   &from.GetPixel(source.X + (area.X - target.X),
                                  source.Y + (y - target.Y)),
                   area.Width * sizeof(Pixel));
        }

        return;
    }

    /* 16.16 fixed point; a target pixel centre maps to the source position
     * `(i + 0.5) * source / target - 0.5`. */
    const int64_t stepX = (static_cast<int64_t>(source.Width) << 16) /
                          target.Width;
    const int64_t stepY = (static_cast<int64_t>(source.Height) << 16) /
                          target.Height;
    const int maxX = source.Width - 1, maxY = source.Height - 1;

    struct Tap {
        int First, Second, Weight;
    };

    auto columns = std::make_unique_for_overwrite<Tap[]>(area.Width);

    auto tap = [](int64_t position, int max) {
        position = std::clamp<int64_t>(position,
                                       0,
                                       static_cast<int64_t>(max) << 16);
        const int first = static_cast<int>(position >> 16);

        return Tap{first,
                   std::min(first + 1, max),
                   static_cast<int>((position & 0xFFFF) >> 8)};
    };

    for (int x = 0; x < area.Width; x++) {
        const int index = (area.X - target.X) + x;
        columns[x] = tap(index * stepX + (stepX >> 1) - 0x8000, maxX);
    }

    for (int y = area.Y; y < area.Bottom(); y++) {
        const int index = y - target.Y;
        const Tap row = tap(index * stepY + (stepY >> 1) - 0x8000, maxY);
        const Pixel *upper = &from.GetPixel(source.X, source.Y + row.First);
        const Pixel *lower = &from.GetPixel(source.X, source.Y + row.Second);
        Pixel *out = &GetPixel(area.X, y);

        for (int x = 0; x < area.Width; x++) {
            const Tap &column = columns[x];
            const Pixel &a = upper[column.First], &b = upper[column.Second];
            const Pixel &c = lower[column.First], &d = lower[column.Second];
            const int wx = column.Weight, wy = row.Weight;

            auto mix = [wx, wy](int a, int b, int c, int d) {
                const int top = a * (256 - wx) + b * wx;
                const int bottom = c * (256 - wx) + d * wx;

                return static_cast<uint8_t>(
                        (top * (256 - wy) + bottom * wy + 0x8000) >> 16);
            };

            out[x].Red = mix(a.Red, b.Red, c.Red, d.Red);
            out[x].Green = mix(a.Green, b.Green, c.Green, d.Green);
            out[x].Blue = mix(a.Blue, b.Blue, c.Blue, d.Blue);
            out[x].Alpha = 0xFF;
        }
    }
}

void Canvas::StretchNearest(const Canvas &from,
                            const Rect &source,
                            const Rect &target) {
    const Rect area = target.Intersect(ClipRect);

    if (area.Empty() || source.Empty()) {
        return;
    }

    for (int y = area.Y; y < area.Bottom(); y++) {
        const int fromY = source.Y + (y - target.Y) * source.Height /
                                             target.Height;

        for (int x = area.X; x < area.Right(); x++) {
            const int fromX = source.X + (x - target.X) * source.Width /
                                                 target.Width;
            const Pixel &pixel = from.GetPixel(fromX, fromY);

            if (!pixel.IsTransparent()) {
                GetPixel(x, y) = pixel;
            }
        }
    }
}

} // namespace trc
