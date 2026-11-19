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

#ifndef __TRC_CLIENT_UI_CONSOLE_INPUT_LINE_HPP__
#define __TRC_CLIENT_UI_CONSOLE_INPUT_LINE_HPP__

#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"

#include <cstdint>

namespace trc {
namespace Client {

/* The line the player types into. Nobody types during a replay, so all
 * there is to see is the empty field and its blinking caret. */
void DrawInputLine(Canvas &canvas,
                   const Fonts &fonts,
                   const Skin &skin,
                   const Rect &area,
                   uint32_t now);

} // namespace Client
} // namespace trc

#endif
