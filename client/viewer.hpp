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

#ifndef __TRC_CLIENT_VIEWER_HPP__
#define __TRC_CLIENT_VIEWER_HPP__

#include "graphics/canvas.hpp"
#include "playback/session.hpp"
#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "scene/mapview.hpp"
#include "scene/overlay.hpp"
#include "ui/console_panel.hpp"
#include "ui/layout.hpp"
#include "ui/sidebar.hpp"

#include <memory>

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

    Session Playback;

private:
    const Version &Data;
    Fonts Text;
    Skin Pieces;
    MapRenderer Renderer;
    MapOverlay Overlay;
    ConsolePanel Console;
    Sidebar Side;
    Layout Areas;
    int ConsoleHeight = Layout::MinimumConsoleHeight;
    Canvas MapCanvas;
    MapFrame Frame;
    std::unique_ptr<Canvas> Window;
};

} // namespace Client
} // namespace trc

#endif
