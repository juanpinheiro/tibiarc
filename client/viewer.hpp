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

#ifndef __TRC_CLIENT_VIEWER_HPP__
#define __TRC_CLIENT_VIEWER_HPP__

#include "graphics/canvas.hpp"
#include "playback/session.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "scene/mapview.hpp"
#include "scene/overlay.hpp"
#include "ui/console_panel.hpp"
#include "ui/hotspots.hpp"
#include "ui/layout.hpp"
#include "ui/sidebar.hpp"

#include <memory>
#include <optional>

namespace trc {
namespace Client {

/* The whole client window: plays a recording and paints what the game client
 * would have shown at any moment of it. */
class Viewer {
public:
    static constexpr int MinimumWidth = 550;
    static constexpr int MinimumHeight = 450;

    Viewer(const Version &version,
           std::unique_ptr<Recordings::Recording> recording,
           int width,
           int height);

    void Resize(int width, int height);

    /* The height the console would like to have; the game window takes
     * what is left above it. */
    void SetConsoleHeight(int height);

    const Layout &Arrangement() const {
        return Areas;
    }

    int Width() const {
        return Window->Width;
    }

    int Height() const {
        return Window->Height;
    }

    /* Paints the window as of the session's current time. */
    const Canvas &Render();

    /* Whether pressing the given point of the window, as last painted,
     * does something. */
    bool Clickable(int x, int y) const {
        return Spots.At(x, y).has_value() ||
               Spots.ScrollbarAt(x, y).has_value();
    }

    /* The mouse button going down, moving while down, and coming up again,
     * which is how scrollbars are worked: their arrows step, their thumb
     * follows the pointer, and letting go beside the thumb turns a page.
     * `Press` returns whether a scrollbar was there. */
    bool Press(int x, int y);
    void Drag(int x, int y);
    void Release(int x, int y);

    /* Presses a point of the window as last painted: opens and closes the
     * windows of the sidebar, switches between the channels of the
     * console. Returns whether anything was there to press. */
    bool Click(int x, int y);

    /* Turns the wheel over a point of the window as last painted, by
     * notches, down for positive ones: moves the lists of the sidebar and
     * the text of the console. Returns whether anything there moves. */
    bool Scroll(int x, int y, int notches);

    Session Playback;

private:
    Fonts Text;
    Skin Pieces;
    MapRenderer Renderer;
    MapOverlay Overlay;
    ConsolePanel Console;
    Sidebar Side;
    Layout Areas;
    Hotspots Spots;

    /* A scrollbar held down since `Press`. */
    struct Grip {
        ScrollbarSpot Bar;
        ScrollbarParts Parts;
        /* Where on the thumb it was taken, if it was. */
        std::optional<int> ThumbOffset;
    };

    void ScrollTo(const ScrollbarSpot &bar, int value);

    std::optional<Grip> Held;
    int ConsoleHeight = Layout::MinimumConsoleHeight;
    Canvas MapCanvas;
    MapFrame Frame;
    std::unique_ptr<Canvas> Window;
};

} // namespace Client
} // namespace trc

#endif
