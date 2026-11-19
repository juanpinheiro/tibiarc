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

#include "ui/scrollbar.hpp"

using namespace trc;
using namespace trc::Client;

/* A bar 12 wide and 124 tall: two 12 pixel arrows around a 100 pixel
 * track. */
static const Rect Bar{50, 10, 12, 124};

TEST(ScrollbarIsArrowsAroundATrack) {
    const ScrollbarParts parts(Bar, ScrollRange{0, 300, 0, 100});

    EXPECT_EQ(parts.Up.Y, 10);
    EXPECT_EQ(parts.Track.Y, 22);
    EXPECT_EQ(parts.Track.Height, 100);
    EXPECT_EQ(parts.Down.Y, 122);
}

TEST(ThumbCoversThePagesShareOfTheTrack) {
    /* A quarter of everything is in view. */
    const ScrollbarParts parts(Bar, ScrollRange{0, 300, 0, 100});

    EXPECT_EQ(parts.Thumb.Height, 25);
    EXPECT_EQ(parts.Thumb.Y, 22);
}

TEST(ThumbReachesTheEndOfTheTrackAtTheLastValue) {
    const ScrollbarParts parts(Bar, ScrollRange{0, 300, 300, 100});

    EXPECT_EQ(parts.Thumb.Bottom(), parts.Track.Bottom());
}

TEST(ThumbIsNeverSmallerThanItsCaps) {
    const ScrollbarParts parts(Bar, ScrollRange{0, 100000, 0, 10});

    EXPECT_EQ(parts.Thumb.Height, 12);
}

TEST(NothingToScrollMeansNoThumb) {
    const ScrollbarParts parts(Bar, ScrollRange{0, 0, 0, 100});

    EXPECT_EQ(parts.Thumb.Empty(), true);
}

TEST(DraggingTheThumbCoversTheWholeRange) {
    const ScrollRange range{0, 300, 0, 100};
    const ScrollbarParts parts(Bar, range);

    /* As in the client, the last value is only reached by pulling the
     * thumb a pixel past the end of the track. */
    EXPECT_EQ(parts.ValueAt(range, 0), 0);
    EXPECT_EQ(parts.ValueAt(range, 75), 297);
    EXPECT_EQ(parts.ValueAt(range, 76), 300);
    EXPECT_EQ(parts.ValueAt(range, -40), 0);
    EXPECT_EQ(parts.ValueAt(range, 500), 300);
}
