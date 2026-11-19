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

#ifndef __TRC_CLIENT_SCENE_ANIMATION_HPP__
#define __TRC_CLIENT_SCENE_ANIMATION_HPP__

#include "game/creature.hpp"
#include "game/missile.hpp"

#include <cstdint>

namespace trc {
namespace Client {

/* The game client advances its animations on a fixed 100 ms timer. Everything
 * that animates is expressed in these ticks, derived from the playback time
 * so that any moment of a recording can be rendered on its own. */
namespace Clock {
static constexpr uint32_t TickLength = 100;

constexpr uint32_t Ticks(uint32_t milliseconds) {
    return milliseconds / TickLength;
}

/* Ticks that have fired since `since`, both in milliseconds. */
constexpr uint32_t TicksSince(uint32_t now, uint32_t since) {
    return now >= since ? Ticks(now) - Ticks(since) : 0;
}
} // namespace Clock

struct WalkPose {
    /* Pixel offset from the creature's field; shrinks to zero as the step
     * completes. */
    int OffsetX = 0;
    int OffsetY = 0;
    bool Walking = false;
    Creature::Direction Heading = Creature::Direction::South;

    /* The frame of a walking animation with `phases` frames, the first of
     * which is the standing pose. */
    int Phase(int phases, bool arrived) const;
};

WalkPose EvaluateWalk(const Creature &creature, uint32_t now);

struct MissilePose {
    bool Alive = false;
    /* Pixel offset from the target field. */
    int OffsetX = 0;
    int OffsetY = 0;
    /* One of eight headings, clockwise from north. */
    int Direction = 0;
};

MissilePose EvaluateMissile(const Missile &missile, uint32_t now);

} // namespace Client
} // namespace trc

#endif
