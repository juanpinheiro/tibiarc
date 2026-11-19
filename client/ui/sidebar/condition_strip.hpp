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

#ifndef __TRC_CLIENT_UI_SIDEBAR_CONDITION_STRIP_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_CONDITION_STRIP_HPP__

#include "game/gamestate.hpp"
#include "graphics/canvas.hpp"
#include "ui/widgets.hpp"

namespace trc {
namespace Client {

/* The strip below the inventory with a small picture for everything that
 * ails or aids the player: poison, fire, haste, a skull and so on. They are
 * lined up from the left without gaps. The strip's top-left corner is
 * (x, y). */
void DrawConditionStrip(Canvas &canvas,
                        const Widgets &widgets,
                        int x,
                        int y,
                        const Gamestate &gamestate);

} // namespace Client
} // namespace trc

#endif
