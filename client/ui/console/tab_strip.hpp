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

#ifndef __TRC_CLIENT_UI_CONSOLE_TAB_STRIP_HPP__
#define __TRC_CLIENT_UI_CONSOLE_TAB_STRIP_HPP__

#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "world/console.hpp"

#include <cstdint>

namespace trc {
namespace Client {

/* The row of tabs along the top of the console, one per open channel in
 * the order they were opened. Returns whether there were more tabs than
 * fit in `area`; those are left out. */
bool DrawTabStrip(Canvas &canvas,
                  const Fonts &fonts,
                  const Skin &skin,
                  const Rect &area,
                  const ConsoleLog &log,
                  uint32_t now);

} // namespace Client
} // namespace trc

#endif
