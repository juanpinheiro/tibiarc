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

#ifndef __TRC_CLIENT_UI_SIDEBAR_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_HPP__

#include "assets/versions.hpp"
#include "game/gamestate.hpp"
#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "world/world.hpp"

#include <cstdint>

namespace trc {
namespace Client {

/* The column along the right edge of the window: automap, health and mana,
 * inventory, and the stack of open windows (containers, skills, battle and
 * VIP lists). */
class Sidebar {
public:
    Sidebar(const Version &version, const Fonts &fonts, const Skin &skin)
        : Data(version), Text(fonts), Pieces(skin) {
    }

    void Draw(Canvas &canvas,
              const Rect &area,
              const Gamestate &gamestate,
              const World &world,
              uint32_t now);

private:
    const Version &Data;
    const Fonts &Text;
    const Skin &Pieces;
};

} // namespace Client
} // namespace trc

#endif
