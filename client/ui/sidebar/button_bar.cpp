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

#include "ui/sidebar/button_bar.hpp"

#include <string_view>

namespace trc {
namespace Client {

namespace {

constexpr int ButtonsY = 3;
constexpr int SkillsX = 8, BattleX = 45, VIPX = 82, LogoutX = 124;

} // namespace

void DrawButtonBar(Canvas &canvas,
                   const Widgets &widgets,
                   int x,
                   int y,
                   const OpenWindows &open) {
    const auto toggle = [&](int left, std::string_view caption, bool down) {
        widgets.TextButton(canvas,
                           ButtonSkins::Narrow,
                           x + left,
                           y + ButtonsY,
                           caption,
                           down);
    };

    toggle(SkillsX, "Skills", open.Skills);
    toggle(BattleX, "Battle", open.Battle);
    toggle(VIPX, "VIP", open.VIP);

    widgets.TextButton(canvas,
                       ButtonSkins::Regular,
                       x + LogoutX,
                       y + ButtonsY,
                       "Logout");
}

} // namespace Client
} // namespace trc
