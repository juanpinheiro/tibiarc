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

#ifndef __TRC_CLIENT_UI_SIDEBAR_WINDOW_STACK_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_WINDOW_STACK_HPP__

#include "graphics/canvas.hpp"
#include "ui/sidebar/content_window.hpp"
#include "ui/widgets.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace trc {
namespace Client {

/* The column of content windows below the fixed part of the sidebar. The
 * windows sit one under another without gaps and the column never scrolls:
 * when a new window doesn't fit, the ones above it give way. */
class WindowStack {
public:
    explicit WindowStack(int height) : Height(height) {
    }

    /* Opens a window at the bottom the way the client does. If there isn't
     * room for it the windows above are made smaller, starting with the
     * lowest, and those that cannot shrink enough are closed. */
    void Open(std::unique_ptr<ContentWindow> window);

    /* Opens a window at the bottom if that takes nothing away from the
     * windows opened with `Open`: only others opened this way shrink for
     * it, and none is closed. Returns whether it was opened. */
    bool OpenIfRoom(std::unique_ptr<ContentWindow> window);

    /* Draws the column with its top-left corner at (x, y). */
    void Draw(Canvas &canvas,
              Widgets &widgets,
              int x,
              int y,
              uint32_t now) const;

private:
    struct Entry {
        std::unique_ptr<ContentWindow> Window;
        /* Height of the whole window. */
        int Height;
        /* Opened only because there was room. */
        bool Optional;

        int MinimumHeight() const {
            return Window->MinimumHeight() + ContentWindow::BarsHeight;
        }
    };

    int FreeHeight() const;

    /* How much the optional windows could give up between them. */
    int Slack() const;

    void Place(std::unique_ptr<ContentWindow> window, bool optional);

    void DrawFreeArea(Canvas &canvas,
                      const Skin &skin,
                      int x,
                      int y,
                      int top) const;

    std::vector<Entry> Entries;
    int Height;
};

} // namespace Client
} // namespace trc

#endif
