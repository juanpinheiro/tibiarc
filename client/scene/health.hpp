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

#ifndef __TRC_CLIENT_SCENE_HEALTH_HPP__
#define __TRC_CLIENT_SCENE_HEALTH_HPP__

#include "common/pixel.hpp"

namespace trc {
namespace Client {

/* The colour the client paints a creature's health in, wherever it shows
 * it, by the percentage that is left. */
inline Pixel HealthColor(int percent) {
    if (percent >= 0 && percent <= 3) {
        return Pixel(0x60, 0, 0);
    } else if (percent >= 4 && percent <= 9) {
        return Pixel(0xC0, 0, 0);
    } else if (percent >= 10 && percent <= 29) {
        return Pixel(0xC0, 0x30, 0x30);
    } else if (percent >= 30 && percent <= 59) {
        return Pixel(0xC0, 0xC0, 0);
    } else if (percent < 60 || percent > 94) {
        return Pixel(0, 0xC0, 0);
    }

    return Pixel(0x60, 0xC0, 0x60);
}

} // namespace Client
} // namespace trc

#endif
