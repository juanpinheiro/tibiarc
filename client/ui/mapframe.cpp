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

#include "ui/mapframe.hpp"

namespace trc {
namespace Client {

void DrawMapFrame(Canvas &canvas, const Skin &skin, const Layout &layout) {
    const Rect &frame = layout.MapFrame;
    const Rect &view = layout.View;
    const int texture = Skin::Width(Skin::Background);

    /* Edges of the sunken area, in frame coordinates. */
    const int top = view.Y - 1, left = view.X - 1;
    const int right = view.X + view.Width + 1;
    const int bottom = view.Y + view.Height + 1;

    const auto place = [&](int id, int x, int y, int w, int h, int sx, int sy) {
        skin.Tile(canvas, id, Rect{frame.X + x, frame.Y + y, w, h}, sx, sy);
    };

    place(Skin::LineTop, left, top, view.Width + 2, 1, 0, 0);
    place(Skin::LineLeft, left, view.Y, 1, view.Height + 1, 0, 0);
    place(Skin::LineRight, view.X + view.Width, view.Y, 1, view.Height + 1, 0, 0);
    place(Skin::LineBottom, view.X, view.Y + view.Height, view.Width, 1, 0, 0);

    /* The background is one continuous texture anchored at the frame's
     * corner, laid around the view in four pieces. */
    place(Skin::Background, 0, 0, frame.Width, top, 0, 0);
    place(Skin::Background, 0, top, left, frame.Height - top, 0, top % texture);
    place(Skin::Background,
          right,
          top,
          frame.Width - right,
          frame.Height - top,
          right % texture,
          top % texture);
    place(Skin::Background,
          left,
          bottom,
          right - left,
          frame.Height - bottom,
          left % texture,
          bottom % texture);
}

void DrawSplitter(Canvas &canvas, const Skin &skin, const Layout &layout) {
    const Rect &bar = layout.Splitter;

    skin.Tile(canvas, Skin::LineBottom, Rect{bar.X, bar.Y, bar.Width, 1});
    skin.Tile(canvas,
              Skin::Background,
              Rect{bar.X, bar.Y + 1, bar.Width, bar.Height - 2});
    skin.Tile(canvas,
              Skin::LineTop,
              Rect{bar.X, bar.Y + bar.Height - 1, bar.Width, 1});
}

} // namespace Client
} // namespace trc
