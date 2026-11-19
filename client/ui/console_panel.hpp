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

#ifndef __TRC_CLIENT_UI_CONSOLE_PANEL_HPP__
#define __TRC_CLIENT_UI_CONSOLE_PANEL_HPP__

#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "world/console.hpp"

#include <cstdint>

namespace trc {
namespace Client {

/* The console below the game window: channel tabs, the text of the active
 * channel and the input line. */
class ConsolePanel {
public:
    ConsolePanel(const Fonts &fonts, const Skin &skin)
        : Text(fonts), Pieces(skin) {
    }

    void Draw(Canvas &canvas,
              const Rect &area,
              const ConsoleLog &log,
              uint32_t now);

private:
    const Fonts &Text;
    const Skin &Pieces;
};

} // namespace Client
} // namespace trc

#endif
