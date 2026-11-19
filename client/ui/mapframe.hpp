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

#ifndef __TRC_CLIENT_UI_MAPFRAME_HPP__
#define __TRC_CLIENT_UI_MAPFRAME_HPP__

#include "graphics/canvas.hpp"
#include "resources/skin.hpp"
#include "ui/layout.hpp"

namespace trc {
namespace Client {

/* The textured surround of the game window with the sunken edge around the
 * map picture. Leaves the view itself untouched. */
void DrawMapFrame(Canvas &canvas, const Skin &skin, const Layout &layout);

/* The bar between the game window and the console. */
void DrawSplitter(Canvas &canvas, const Skin &skin, const Layout &layout);

} // namespace Client
} // namespace trc

#endif
