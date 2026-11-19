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

#ifndef __TRC_CLIENT_UI_SIDEBAR_CONTENT_WINDOW_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_CONTENT_WINDOW_HPP__

#include "graphics/canvas.hpp"
#include "ui/widgets.hpp"

#include <cstdint>
#include <string_view>

namespace trc {
namespace Client {

/* One of the windows stacked below the fixed part of the sidebar. They all
 * share a frame: a title bar with a small picture, a title and buttons to
 * minimise and close the window, a thin border along the other sides, and
 * a grip in the bottom-left corner. What differs is the body, which is a
 * list of some sort next to a scrollbar.
 *
 * The windows are only ever as wide as the sidebar; heights are those of
 * the body unless said otherwise. */
class ContentWindow {
public:
    /* What the title bar and the bottom border add to the body's height. */
    static constexpr int BarsHeight = 19;

    virtual ~ContentWindow() = default;

    /* The least height the body can be shrunk to. */
    virtual int MinimumHeight() const {
        return Widgets::ScrollbarMinimumHeight;
    }

    /* The height the body is given when the window is opened. */
    virtual int InitialHeight() const = 0;

    /* Where the button that closes a window is, for one whose top-left
     * corner is (x, y). */
    static Rect CloseButtonArea(int x, int y);

    /* Draws the window with its top-left corner at (x, y) and its body at
     * the given height. */
    void Draw(Canvas &canvas,
              Widgets &widgets,
              int x,
              int y,
              int bodyHeight,
              uint32_t now) const;

protected:
    /* Width of the list, which is what the scrollbar leaves of the body. */
    static constexpr int ListWidth = 156;

    /* Clamps a wanted body height to what the window can shrink to. */
    int AtLeastMinimum(int height) const;

    virtual std::string_view Title() const = 0;

    /* Draws the 12 by 12 picture at the left end of the title bar. */
    virtual void DrawIcon(Canvas &canvas,
                          Widgets &widgets,
                          int x,
                          int y,
                          uint32_t now) const = 0;

    /* Whether the title bar has a button that leads to a parent. */
    virtual bool HasUpButton() const {
        return false;
    }

    /* The height of everything the list holds. */
    virtual int ContentHeight() const = 0;

    /* How much of the list is in view at a time. */
    virtual int PageHeight(int bodyHeight) const {
        return bodyHeight;
    }

    /* Draws the list into `area`, to which the canvas is clipped. */
    virtual void DrawList(Canvas &canvas,
                          Widgets &widgets,
                          const Rect &area,
                          uint32_t now) const = 0;
};

} // namespace Client
} // namespace trc

#endif
