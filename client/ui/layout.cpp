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

#include "ui/layout.hpp"

#include <algorithm>
#include <cmath>

namespace trc {
namespace Client {

Layout Layout::Compute(int width,
                       int height,
                       int consoleHeight,
                       bool noStretching) {
    const int minimumMap = MinimumViewHeight + MapBorder * 2;
    const int inner = width - SidebarWidth;
    Layout result;

    consoleHeight = std::max(consoleHeight, MinimumConsoleHeight);

    const int largestMap = std::max(
            minimumMap,
            height - MinimumConsoleHeight - SplitterHeight);
    const int mapHeight = std::clamp(height - SplitterHeight - consoleHeight,
                                     minimumMap,
                                     largestMap);

    /* The widest view the height allows, or all the width there is. */
    int viewWidth = std::min(inner - MapBorder * 2,
                             (mapHeight - MapBorder * 2) * 15 / 11 + 2);

    if (noStretching) {
        viewWidth = std::min(viewWidth, 480);
    }

    const int viewHeight = static_cast<int>(
            std::floor(static_cast<float>(viewWidth) * 0.73333335f));
    const int frameHeight = viewHeight + MapBorder * 2;

    result.MapFrame = Rect{0, 0, inner, frameHeight};
    result.View = Rect{(inner - MapBorder * 2 - viewWidth) / 2 + MapBorder,
                       MapBorder,
                       viewWidth,
                       viewHeight};
    result.Splitter = Rect{0, frameHeight, inner, SplitterHeight};
    result.Console = Rect{0,
                          frameHeight + SplitterHeight,
                          inner,
                          height - SplitterHeight - frameHeight};
    result.Sidebar = Rect{inner, 0, SidebarWidth, height};

    return result;
}

} // namespace Client
} // namespace trc
