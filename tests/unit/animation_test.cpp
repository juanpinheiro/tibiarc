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

#include "check.hpp"

#include "scene/animation.hpp"

using namespace trc;
using namespace trc::Client;

static Creature WalkingEast(uint32_t start, uint32_t end) {
    Creature creature{};

    creature.Heading = Creature::Direction::South;
    creature.Walk.StartTick = start;
    creature.Walk.EndTick = end;
    creature.Walk.SettleTick = end;
    creature.Walk.DeltaX = 1;
    creature.Walk.DeltaY = 0;
    creature.Walk.Heading = Creature::Direction::East;

    return creature;
}

TEST(ClockCountsWholeTicks) {
    EXPECT_EQ(Clock::Ticks(0), 0u);
    EXPECT_EQ(Clock::Ticks(99), 0u);
    EXPECT_EQ(Clock::Ticks(100), 1u);
    EXPECT_EQ(Clock::TicksSince(1050, 990), 1u);
    EXPECT_EQ(Clock::TicksSince(500, 900), 0u);
}

TEST(StepStartsAFullFieldBehind) {
    const Creature creature = WalkingEast(1000, 1500);
    const WalkPose pose = EvaluateWalk(creature, 1000);

    EXPECT_EQ(pose.OffsetX, -32);
    EXPECT_EQ(pose.OffsetY, 0);
    EXPECT_EQ(pose.Walking, true);
    EXPECT_EQ(static_cast<int>(pose.Heading),
              static_cast<int>(Creature::Direction::East));
}

TEST(StepClosesInLinearly) {
    const Creature creature = WalkingEast(1000, 1500);

    EXPECT_EQ(EvaluateWalk(creature, 1250).OffsetX, -16);
    EXPECT_EQ(EvaluateWalk(creature, 1375).OffsetX, -8);
}

TEST(CreatureRestsOnceArrived) {
    const Creature creature = WalkingEast(1000, 1500);
    const WalkPose pose = EvaluateWalk(creature, 1600);

    EXPECT_EQ(pose.OffsetX, 0);
    EXPECT_EQ(pose.Walking, false);
    EXPECT_EQ(static_cast<int>(pose.Heading),
              static_cast<int>(Creature::Direction::South));
    EXPECT_EQ(pose.Phase(3, true), 0);
}

TEST(WalkingFramesFollowTheDistanceLeft) {
    WalkPose pose;

    /* Every eight pixels advance the animation, skipping the standing
     * frame. */
    pose.OffsetX = -32;
    EXPECT_EQ(pose.Phase(3, false), 1);
    pose.OffsetX = -24;
    EXPECT_EQ(pose.Phase(3, false), 2);
    pose.OffsetX = -16;
    EXPECT_EQ(pose.Phase(3, false), 1);
}
