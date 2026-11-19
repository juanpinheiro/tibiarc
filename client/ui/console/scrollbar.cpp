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

#include "ui/console/scrollbar.hpp"

#include <algorithm>

namespace trc {
namespace Client {

/* Skins of the vertical scrollbar. */
enum ScrollbarSkin : int {
    Track = 153,
    ThumbTop = 154,
    ThumbBody = 155,
    ThumbBottom = 156,
    ArrowUp = 157,
    ArrowDown = 159,
};

/* The track never gets shorter than this, however small the scrollbar. */
static constexpr int MinimumTrackHeight = 14;

static void DrawThumb(Canvas &canvas,
                      const Skin &skin,
                      const Rect &track,
                      const ScrollRange &range) {
    const int caps = Skin::Height(ThumbTop) + Skin::Height(ThumbBottom);
    const int span = range.Max - range.Min;

    /* The thumb covers the same share of the track as the page does of
     * everything there is to see, but never less than its two end caps. */
    const float share = static_cast<float>(range.Page) /
                        static_cast<float>(span + range.Page);
    const int scaled = static_cast<int>(static_cast<float>(track.Height) *
                                        std::min(share, 1.0f));
    const int size = std::max(scaled, caps);
    const int room = track.Height - size;
    const int top = track.Y +
                    std::min((range.Value - range.Min) * room / span, room);

    skin.Draw(canvas, ThumbTop, track.X, top);
    skin.Tile(canvas,
              ThumbBody,
              Rect{track.X,
                   top + Skin::Height(ThumbTop),
                   track.Width,
                   size - caps});
    skin.Draw(canvas,
              ThumbBottom,
              track.X,
              top + size - Skin::Height(ThumbBottom));
}

void DrawScrollbar(Canvas &canvas,
                   const Skin &skin,
                   const Rect &area,
                   const ScrollRange &range) {
    const int arrows = Skin::Height(ArrowUp) + Skin::Height(ArrowDown);
    const Rect track{area.X,
                     area.Y + Skin::Height(ArrowUp),
                     Skin::Width(Track),
                     std::max(area.Height - arrows, MinimumTrackHeight)};

    skin.Draw(canvas, ArrowUp, area.X, area.Y);
    skin.Tile(canvas, Track, track);
    skin.Draw(canvas, ArrowDown, area.X, track.Bottom());

    /* A bar with nothing to scroll is a bare track. */
    if (range.Min != range.Max) {
        DrawThumb(canvas, skin, track, range);
    }
}

} // namespace Client
} // namespace trc
