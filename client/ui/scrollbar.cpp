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

#include "ui/scrollbar.hpp"

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

int ScrollRange::Clamp(int value) const {
    return std::clamp(value, std::min(Min, Max), std::max(Min, Max));
}

ScrollbarParts::ScrollbarParts(const Rect &area, const ScrollRange &range) {
    const int arrows = Skin::Height(ArrowUp) + Skin::Height(ArrowDown);

    Up = Rect{area.X, area.Y, Skin::Width(ArrowUp), Skin::Height(ArrowUp)};
    Track = Rect{area.X,
                 Up.Bottom(),
                 Skin::Width(ScrollbarSkin::Track),
                 std::max(area.Height - arrows, MinimumTrackHeight)};
    Down = Rect{area.X,
                Track.Bottom(),
                Skin::Width(ArrowDown),
                Skin::Height(ArrowDown)};

    if (!range.Scrollable()) {
        return;
    }

    const int caps = Skin::Height(ThumbTop) + Skin::Height(ThumbBottom);
    const int span = range.Max - range.Min;

    /* The thumb covers the same share of the track as the page does of
     * everything there is to see, but never less than its two end caps. */
    const float share = static_cast<float>(range.Page) /
                        static_cast<float>(span + range.Page);
    const int scaled = static_cast<int>(static_cast<float>(Track.Height) *
                                        std::min(share, 1.0f));
    const int size = std::max(scaled, caps);
    const int room = Track.Height - size;

    Thumb = Rect{Track.X,
                 Track.Y +
                         std::min((range.Value - range.Min) * room / span, room),
                 Track.Width,
                 size};
}

int ScrollbarParts::ValueAt(const ScrollRange &range, int offset) const {
    const int span = range.Max - range.Min;
    const int room = Track.Height - Thumb.Height;

    return range.Clamp(offset * (span + 1) / (room + 1) + range.Min);
}

void DrawScrollbar(Canvas &canvas,
                   const Skin &skin,
                   const Rect &area,
                   const ScrollRange &range) {
    const ScrollbarParts parts(area, range);
    const Rect &thumb = parts.Thumb;

    skin.Draw(canvas, ArrowUp, parts.Up.X, parts.Up.Y);
    skin.Tile(canvas, Track, parts.Track);
    skin.Draw(canvas, ArrowDown, parts.Down.X, parts.Down.Y);

    /* A bar with nothing to scroll is a bare track. */
    if (thumb.Empty()) {
        return;
    }

    skin.Draw(canvas, ThumbTop, thumb.X, thumb.Y);
    skin.Tile(canvas,
              ThumbBody,
              Rect{thumb.X,
                   thumb.Y + Skin::Height(ThumbTop),
                   thumb.Width,
                   thumb.Height - Skin::Height(ThumbTop) -
                           Skin::Height(ThumbBottom)});
    skin.Draw(canvas,
              ThumbBottom,
              thumb.X,
              thumb.Bottom() - Skin::Height(ThumbBottom));
}

} // namespace Client
} // namespace trc
