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

#include "ui/sidebar/window_stack.hpp"

#include "ui/layout.hpp"

#include <algorithm>

namespace trc {
namespace Client {

namespace {

constexpr int Width = Layout::SidebarWidth;

/* The room below the last window is a panel of its own, framed by a raised
 * border of this thickness. */
constexpr int Border = 2;
constexpr int TopLeftSkin = 26;
constexpr int TopRightSkin = 27;
constexpr int BottomLeftSkin = 28;
constexpr int BottomRightSkin = 29;
constexpr int LeftSkin = 30;
constexpr int RightSkin = 31;
constexpr int TopSkin = 32;
constexpr int BottomSkin = 33;

} // namespace

int WindowStack::FreeHeight() const {
    int free = Height;

    for (const Entry &entry : Entries) {
        free -= entry.Height;
    }

    return free;
}

int WindowStack::Slack() const {
    int slack = 0;

    for (const Entry &entry : Entries) {
        if (entry.Optional) {
            slack += entry.Height - entry.MinimumHeight();
        }
    }

    return slack;
}

void WindowStack::Place(std::unique_ptr<ContentWindow> window,
                        const Closing &closing,
                        uint32_t key,
                        bool optional) {
    Entry entry{std::move(window), closing, key, 0, optional};
    int wanted = entry.Window->InitialHeight() + ContentWindow::BarsHeight;
    int free = FreeHeight();

    if (free < wanted) {
        /* The newcomer makes do with the room there is, as long as that is
         * at least the smallest it can be. */
        const int needed = std::max(free, entry.MinimumHeight());

        for (size_t i = Entries.size(); i > 0 && free < needed; i--) {
            Entry &last = Entries[i - 1];

            if (optional && !last.Optional) {
                continue;
            }

            const int shrunk = std::max(last.Height - (needed - free),
                                        last.MinimumHeight());

            free += last.Height - shrunk;
            last.Height = shrunk;

            if (free < needed && !optional) {
                free += last.Height;
                Entries.erase(Entries.begin() + static_cast<long>(i - 1));
            }
        }

        wanted = std::min(free, wanted);
    }

    /* A column too short for even one window stays empty. */
    if (wanted >= entry.MinimumHeight()) {
        entry.Height = wanted;
        Entries.push_back(std::move(entry));
    }
}

void WindowStack::Open(std::unique_ptr<ContentWindow> window,
                       const Closing &closing,
                       uint32_t key) {
    Place(std::move(window), closing, key, false);
}

bool WindowStack::OpenIfRoom(std::unique_ptr<ContentWindow> window,
                             const Closing &closing,
                             uint32_t key) {
    const int minimum = window->MinimumHeight() + ContentWindow::BarsHeight;

    if (FreeHeight() + Slack() < minimum) {
        return false;
    }

    Place(std::move(window), closing, key, true);

    return true;
}

void WindowStack::DrawFreeArea(Canvas &canvas,
                               const Skin &skin,
                               int x,
                               int y,
                               int top) const {
    const int free = Height - top;
    const int inner = Width - Border * 2;
    const int right = x + Width - Border;

    if (free <= 0) {
        return;
    }

    /* With too little room the border comes first: its top edge, then its
     * bottom edge, and only then anything in between. */
    const int upper = std::min(free, Border);
    const int lower = std::min(free - Border, Border);
    const int middle = free - Border * 2;

    skin.Tile(canvas, TopLeftSkin, Rect{x, y + top, Border, upper});
    skin.Tile(canvas, TopSkin, Rect{x + Border, y + top, inner, upper});
    skin.Tile(canvas, TopRightSkin, Rect{right, y + top, Border, upper});

    if (lower > 0) {
        const int bottom = y + Height - lower;

        skin.Tile(canvas, BottomLeftSkin, Rect{x, bottom, Border, lower});
        skin.Tile(canvas, BottomSkin, Rect{x + Border, bottom, inner, lower});
        skin.Tile(canvas, BottomRightSkin, Rect{right, bottom, Border, lower});
    }

    if (middle > 0) {
        const int from = y + top + Border;

        /* The texture carries on from the windows above. */
        skin.Tile(canvas, LeftSkin, Rect{x, from, Border, middle});
        skin.Tile(canvas,
                  Skin::Background,
                  Rect{x + Border, from, inner, middle},
                  0,
                  top % Skin::Height(Skin::Background));
        skin.Tile(canvas, RightSkin, Rect{right, from, Border, middle});
    }
}

void WindowStack::Draw(Canvas &canvas,
                       Widgets &widgets,
                       Hotspots &hotspots,
                       ScrollPositions &scrolls,
                       int x,
                       int y,
                       uint32_t now) const {
    const ClipScope clip(canvas, Rect{x, y, Width, Height});
    int top = 0;

    for (const Entry &entry : Entries) {
        const int body = entry.Height - ContentWindow::BarsHeight;

        entry.Window->Draw(canvas,
                           widgets,
                           hotspots,
                           entry.Key,
                           x,
                           y + top,
                           body,
                           scrolls[entry.Key],
                           now);
        hotspots.AddWheel(ContentWindow::Area(x, y + top, body),
                          Gesture::ScrollWindow,
                          entry.Key);
        hotspots.Add(ContentWindow::CloseButtonArea(x, y + top),
                     entry.Close.What,
                     entry.Close.Target);
        top += entry.Height;
    }

    DrawFreeArea(canvas, widgets.Pieces, x, y, top);
}

} // namespace Client
} // namespace trc
