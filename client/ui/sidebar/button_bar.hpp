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

#ifndef __TRC_CLIENT_UI_SIDEBAR_BUTTON_BAR_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_BUTTON_BAR_HPP__

#include "graphics/canvas.hpp"
#include "ui/widgets.hpp"

namespace trc {
namespace Client {

constexpr int ButtonBarHeight = 26;

/* Which of the windows that have a button of their own are open. */
struct OpenWindows {
    bool Skills = false;
    bool Battle = false;
    bool VIP = false;
};

/* The row of buttons at the bottom of the fixed part of the sidebar. Those
 * that open a window stay down for as long as it is open. The row's
 * top-left corner is (x, y). */
void DrawButtonBar(Canvas &canvas,
                   const Widgets &widgets,
                   int x,
                   int y,
                   const OpenWindows &open);

} // namespace Client
} // namespace trc

#endif
