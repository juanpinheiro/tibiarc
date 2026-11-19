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

#ifndef __TRC_CLIENT_UI_SIDEBAR_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_HPP__

#include "assets/versions.hpp"
#include "game/gamestate.hpp"
#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "ui/hotspots.hpp"
#include "ui/sidebar/button_bar.hpp"
#include "ui/sidebar/window_stack.hpp"
#include "ui/widgets.hpp"
#include "world/world.hpp"

#include <cstdint>
#include <unordered_set>

namespace trc {
namespace Client {

/* The column along the right edge of the window. Its upper part is fixed:
 * automap, health and mana, inventory and a row of buttons, in a frame of
 * their own. Below it the open windows are stacked: those of the containers
 * the player has opened, followed by the skills, battle and VIP lists as
 * far as they are wanted and there is room for them. */
class Sidebar {
public:
    Sidebar(const Version &version, const Fonts &fonts, const Skin &skin)
        : Tools(version, fonts, skin) {
    }

    /* Paints the sidebar, noting what in it can be pressed. */
    void Draw(Canvas &canvas,
              const Rect &area,
              const Gamestate &gamestate,
              const World &world,
              Hotspots &hotspots,
              uint32_t now);

    /* Closes the window of a container, told by the serial of the window.
     * The container stays open in the game, and shows again the next time
     * the recording opens it. */
    void DismissContainer(uint32_t serial) {
        Dismissed.insert(serial);
    }

    /* Moves the list of a window by notches of the wheel, down for
     * positive ones. */
    void ScrollWindow(uint32_t key, int notches) {
        Scrolls[key] += notches * Steps[key];
    }

private:
    /* Opens the windows to show in `stack`, and tells which of those that
     * have a button made it. */
    OpenWindows OpenContentWindows(WindowStack &stack,
                                   const Gamestate &gamestate,
                                   const World &world) const;

    /* Draws the fixed part with its top-left corner at (x, y). */
    void DrawFixedPart(Canvas &canvas,
                       int x,
                       int y,
                       const Gamestate &gamestate,
                       const World &world,
                       const OpenWindows &open,
                       Hotspots &hotspots,
                       uint32_t now);

    Widgets Tools;
    /* Container windows the viewer has closed. */
    std::unordered_set<uint32_t> Dismissed;
    /* Where the viewer has scrolled each window to, and how far a notch of
     * the wheel moves it, as of the last time it was painted. */
    WindowStack::ScrollPositions Scrolls;
    std::unordered_map<uint32_t, int> Steps;
};

} // namespace Client
} // namespace trc

#endif
