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

#include "ui/sidebar/vip_window.hpp"

#include "scene/animation.hpp"

#include <algorithm>

namespace trc {
namespace Client {

namespace {

constexpr int IconSkin = 143;

constexpr int RowHeight = 16;
constexpr int NameX = 20, NameY = 4;
constexpr int NameMaximumWidth = 132;

/* Room the list counts on top of its rows. */
constexpr int ExtraHeight = 10;

/* A buddy who has just logged in stands out for this many ticks. */
constexpr uint32_t NewsTicks = 10;

constexpr int NameFont = 4;
const Pixel OnlineColor(96, 248, 96);
const Pixel OfflineColor(248, 96, 96);
const Pixel NewsColor(248, 248, 248);

bool IsNews(const World::Buddy &buddy, uint32_t now) {
    return buddy.Online && buddy.ChangeTick &&
           Clock::TicksSince(now, *buddy.ChangeTick) < NewsTicks;
}

} // namespace

VIPWindow::VIPWindow(const std::map<uint32_t, World::Buddy> &buddies) {
    for (const auto &[id, buddy] : buddies) {
        Buddies.push_back(&buddy);
    }

    std::stable_sort(Buddies.begin(),
                     Buddies.end(),
                     [](const World::Buddy *a, const World::Buddy *b) {
                         if (a->Online != b->Online) {
                             return a->Online;
                         }

                         return a->Name < b->Name;
                     });
}

int VIPWindow::InitialHeight() const {
    return AtLeastMinimum(ContentHeight());
}

std::string_view VIPWindow::Title() const {
    return "VIP";
}

void VIPWindow::DrawIcon(Canvas &canvas,
                         Widgets &widgets,
                         int x,
                         int y,
                         [[maybe_unused]] uint32_t now) const {
    widgets.Pieces.Draw(canvas, IconSkin, x, y);
}

int VIPWindow::ContentHeight() const {
    return static_cast<int>(Buddies.size()) * RowHeight + ExtraHeight;
}

/* Half an entry. */
int VIPWindow::ScrollStep() const {
    return 8;
}

void VIPWindow::DrawList(Canvas &canvas,
                         Widgets &widgets,
                         const Rect &area,
                         uint32_t now) const {
    int top = area.Y;

    for (const World::Buddy *buddy : Buddies) {
        if (top >= area.Bottom()) {
            break;
        }

        const TextStyle style{NameFont,
                              IsNews(*buddy, now)
                                      ? NewsColor
                                      : (buddy->Online ? OnlineColor
                                                       : OfflineColor),
                              Align::Left};

        widgets.Write(canvas,
                      area.X + NameX,
                      top + NameY,
                      style,
                      widgets.Abbreviate(NameFont,
                                         buddy->Name,
                                         NameMaximumWidth));

        top += RowHeight;
    }
}

} // namespace Client
} // namespace trc
