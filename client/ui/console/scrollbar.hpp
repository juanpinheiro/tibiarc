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

#ifndef __TRC_CLIENT_UI_CONSOLE_SCROLLBAR_HPP__
#define __TRC_CLIENT_UI_CONSOLE_SCROLLBAR_HPP__

#include "graphics/canvas.hpp"
#include "resources/skin.hpp"

namespace trc {
namespace Client {

/* What a scrollbar shows: `Value` lies within [Min, Max], and `Page` is how
 * much of that is visible at once. Nothing can be scrolled when Min equals
 * Max. */
struct ScrollRange {
    int Min;
    int Max;
    int Value;
    int Page;
};

/* A vertical scrollbar filling `area`: an arrow button at either end and a
 * track with a thumb in between. */
void DrawScrollbar(Canvas &canvas,
                   const Skin &skin,
                   const Rect &area,
                   const ScrollRange &range);

} // namespace Client
} // namespace trc

#endif
