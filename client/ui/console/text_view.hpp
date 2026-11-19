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

#ifndef __TRC_CLIENT_UI_CONSOLE_TEXT_VIEW_HPP__
#define __TRC_CLIENT_UI_CONSOLE_TEXT_VIEW_HPP__

#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "world/console.hpp"

namespace trc {
namespace Client {

/* The framed part of the console that shows what was said in a channel,
 * newest at the bottom, with a scrollbar along its right side. */
void DrawConsoleText(Canvas &canvas,
                     const Fonts &fonts,
                     const Skin &skin,
                     const Rect &area,
                     const ConsoleChannel &channel);

} // namespace Client
} // namespace trc

#endif
