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

#include "check.hpp"

#include "ui/hotspots.hpp"

using namespace trc;
using namespace trc::Client;

TEST(NothingToPressOutsideTheSpots) {
    Hotspots spots;

    spots.Add(Rect{10, 10, 20, 20}, Gesture::ToggleSkills);

    EXPECT_EQ(spots.At(9, 15).has_value(), false);
    EXPECT_EQ(spots.At(30, 15).has_value(), false);
    EXPECT_EQ(spots.At(10, 10).has_value(), true);
}

TEST(SpotPaintedLastIsOnTop) {
    Hotspots spots;

    spots.Add(Rect{0, 0, 100, 100}, Gesture::SelectChannel, 1);
    spots.Add(Rect{40, 40, 10, 10}, Gesture::DismissContainer, 7);

    EXPECT_EQ(static_cast<int>(spots.At(45, 45)->What),
              static_cast<int>(Gesture::DismissContainer));
    EXPECT_EQ(spots.At(45, 45)->Target, 7u);
    EXPECT_EQ(spots.At(5, 5)->Target, 1u);
}

TEST(ClearingForgetsEverySpot) {
    Hotspots spots;

    spots.Add(Rect{0, 0, 10, 10}, Gesture::ToggleVIP);
    spots.Clear();

    EXPECT_EQ(spots.At(5, 5).has_value(), false);
}
