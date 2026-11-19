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

#include "ui/sidebar.hpp"

namespace trc {
namespace Client {

void Sidebar::Draw(Canvas &canvas,
                   const Rect &area,
                   [[maybe_unused]] const Gamestate &gamestate,
                   [[maybe_unused]] const World &world,
                   [[maybe_unused]] uint32_t now) {
    ClipScope clip(canvas, area);

    Pieces.Tile(canvas, Skin::Background, area);
}

} // namespace Client
} // namespace trc
