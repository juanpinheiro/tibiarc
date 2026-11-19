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

#ifndef __TRC_CLIENT_UI_CONSOLE_PANEL_HPP__
#define __TRC_CLIENT_UI_CONSOLE_PANEL_HPP__

#include "graphics/canvas.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "ui/console/focus.hpp"
#include "ui/hotspots.hpp"
#include "world/console.hpp"

#include <cstdint>

namespace trc {
namespace Client {

/* The console below the game window: channel tabs, the text of the channel
 * in front and the input line. A recording doesn't tell which channel the
 * player was looking at, so that is up to the viewer: the default one until
 * a tab is pressed. The text stays scrolled to its newest messages. */
class ConsolePanel {
public:
    ConsolePanel(const Fonts &fonts, const Skin &skin)
        : Text(fonts), Pieces(skin) {
    }

    /* Paints the console, noting what in it can be pressed. */
    void Draw(Canvas &canvas,
              const Rect &area,
              const ConsoleLog &log,
              Hotspots &hotspots,
              uint32_t now);

    /* Brings the channel at the given place among the tabs to the front. */
    void SelectChannel(const ConsoleLog &log, size_t place) {
        Focus.Select(log, place);
    }

    /* Scrolls the text of the channel in front so that it ends the given
     * number of lines before the newest. */
    void ScrollBack(const ConsoleLog &log, int lines) {
        Focus.LinesBack(Focus.Front(log)) = lines;
    }

private:
    void DrawBackground(Canvas &canvas, const Rect &area) const;
    void DrawButtons(Canvas &canvas, const Rect &area, bool moreTabs) const;

    const Fonts &Text;
    const Skin &Pieces;
    ConsoleFocus Focus;
};

} // namespace Client
} // namespace trc

#endif
