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

#include "scene/animation.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace trc {
namespace Client {

/* The distance left of a linear move that covers `distance` pixels in
 * `duration` and has `remaining` to go. */
static int RemainingDistance(int64_t remaining, int64_t duration, int distance) {
    if (remaining <= 0 || duration <= 0) {
        return 0;
    }

    return static_cast<int>(
            std::min<int64_t>(distance, (remaining * distance) / duration));
}

WalkPose EvaluateWalk(const Creature &creature, uint32_t now) {
    const auto &walk = creature.Walk;
    WalkPose pose;

    const int64_t remaining = static_cast<int64_t>(walk.EndTick) - now;
    const int64_t duration =
            static_cast<int64_t>(walk.EndTick) - walk.StartTick;

    pose.OffsetX = -walk.DeltaX *
                   RemainingDistance(remaining,
                                     duration,
                                     std::abs(walk.DeltaX) * 32);
    pose.OffsetY = -walk.DeltaY *
                   RemainingDistance(remaining,
                                     duration,
                                     std::abs(walk.DeltaY) * 32);

    /* A creature keeps its walking heading until the step has settled, which
     * is checked on the animation timer rather than continuously. */
    pose.Walking = (walk.SettleTick != 0 || walk.EndTick != 0) &&
                   (pose.OffsetX != 0 || pose.OffsetY != 0 ||
                    now <= walk.SettleTick);
    pose.Heading = pose.Walking ? walk.Heading : creature.Heading;

    return pose;
}

int WalkPose::Phase(int phases, bool arrived) const {
    if (phases <= 1 || (OffsetX == 0 && OffsetY == 0 && arrived)) {
        return 0;
    }

    const int distance = std::max(std::abs(OffsetX), std::abs(OffsetY));

    return (distance / 8) % (phases - 1) + 1;
}

MissilePose EvaluateMissile(const Missile &missile, uint32_t now) {
    MissilePose pose;

    if (missile.Id == 0 || now < missile.StartTick) {
        return pose;
    }

    const int dx = static_cast<int>(missile.Origin.X) - missile.Target.X;
    const int dy = static_cast<int>(missile.Origin.Y) - missile.Target.Y;
    const int toX = -dx, toY = -dy;

    /* Flight time grows with the square root of the distance. */
    const double squared = static_cast<double>(dx) * dx +
                           static_cast<double>(dy) * dy;
    const int64_t duration =
            static_cast<int64_t>(std::sqrt(std::sqrt(squared))) * 150;
    const int64_t remaining =
            static_cast<int64_t>(missile.StartTick) + duration - now;

    const int offsetX = RemainingDistance(remaining,
                                          duration,
                                          std::abs(dx) * 32);
    const int offsetY = RemainingDistance(remaining,
                                          duration,
                                          std::abs(dy) * 32);

    if (offsetX == 0 && offsetY == 0 && remaining < 0) {
        return pose;
    }

    pose.Alive = true;
    pose.OffsetX = (dx > 0) ? offsetX : -offsetX;
    pose.OffsetY = (dy > 0) ? offsetY : -offsetY;

    /* Heading is picked by comparing the slope against tan(22.5°) and
     * tan(67.5°), in 8.8 fixed point. */
    if (toX == 0) {
        pose.Direction = (toY <= 0) ? 0 : 4;
    } else if (toX > 0) {
        const int slope = toY * 0x100;

        if (slope > toX * 0x26A) {
            pose.Direction = 4;
        } else if (slope > toX * 0x6A) {
            pose.Direction = 3;
        } else if (slope > -toX * 0x6A) {
            pose.Direction = 2;
        } else {
            pose.Direction = (slope > -toX * 0x26A) ? 1 : 0;
        }
    } else {
        const int slope = toY * 0x100;

        if (slope > -toX * 0x26A) {
            pose.Direction = 4;
        } else if (slope > -toX * 0x6A) {
            pose.Direction = 5;
        } else if (slope > toX * 0x6A) {
            pose.Direction = 6;
        } else {
            pose.Direction = (slope <= toX * 0x26A) ? 0 : 7;
        }
    }

    return pose;
}

} // namespace Client
} // namespace trc
