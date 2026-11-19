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

#ifndef __TRC_CLIENT_UI_LAYOUT_HPP__
#define __TRC_CLIENT_UI_LAYOUT_HPP__

#include "graphics/canvas.hpp"

namespace trc {
namespace Client {

/* Where the parts of the client window go. The game window keeps the
 * aspect of the map and is as large as the room above the console allows;
 * the sidebar has a fixed width along the right edge. */
struct Layout {
    static constexpr int SidebarWidth = 176;
    static constexpr int SplitterHeight = 5;
    static constexpr int MapBorder = 5;
    static constexpr int MinimumViewWidth = 240;
    static constexpr int MinimumViewHeight = 176;
    static constexpr int MinimumConsoleHeight = 106;

    /* The game window with its frame, and the map picture within it. */
    Rect MapFrame;
    Rect View;
    Rect Splitter;
    Rect Console;
    Rect Sidebar;

    /* `consoleHeight` is the height the console would like to have; it is
     * given what remains once the game window has been fitted. */
    static Layout Compute(int width,
                          int height,
                          int consoleHeight,
                          bool noStretching);
};

} // namespace Client
} // namespace trc

#endif
