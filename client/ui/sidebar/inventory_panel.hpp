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

#ifndef __TRC_CLIENT_UI_SIDEBAR_INVENTORY_PANEL_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_INVENTORY_PANEL_HPP__

#include "game/gamestate.hpp"
#include "graphics/canvas.hpp"
#include "ui/widgets.hpp"

#include <cstdint>

namespace trc {
namespace Client {

constexpr int InventoryPanelHeight = 170;

/* The inventory in its full form: what the player wears and holds, the
 * capacity and soul points left, the conditions the player is under and,
 * to the right, the fight buttons. Its top-left corner is (x, y). */
void DrawInventoryPanel(Canvas &canvas,
                        Widgets &widgets,
                        int x,
                        int y,
                        const Gamestate &gamestate,
                        uint32_t now);

} // namespace Client
} // namespace trc

#endif
