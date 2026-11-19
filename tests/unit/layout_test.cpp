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

#include "ui/layout.hpp"

using trc::Client::Layout;

TEST(DefaultWindowFitsTheMapAboveTheSmallestConsole) {
    const Layout layout = Layout::Compute(600, 450, 0, false);

    /* 424 pixels of width remain beside the sidebar; the height left above
     * the console would allow a wider view than that. */
    EXPECT_EQ(layout.View.Width, 414);
    EXPECT_EQ(layout.View.Height, 303);
    EXPECT_EQ(layout.View.X, 5);
    EXPECT_EQ(layout.View.Y, 5);
    EXPECT_EQ(layout.MapFrame.Height, 313);
    EXPECT_EQ(layout.Splitter.Y, 313);
    EXPECT_EQ(layout.Console.Y, 318);
    EXPECT_EQ(layout.Console.Height, 132);
    EXPECT_EQ(layout.Sidebar.X, 424);
}

TEST(ViewKeepsTheAspectOfTheMap) {
    /* Wide window: the height limits the view, which is then centred. */
    const Layout layout = Layout::Compute(1280, 473, 0, false);

    EXPECT_EQ(layout.View.Width, 482);
    EXPECT_EQ(layout.View.Height, 353);
    EXPECT_EQ(layout.View.X, (1280 - 176 - 10 - 482) / 2 + 5);
}

TEST(ConsoleTakesWhatTheGameWindowLeaves) {
    const Layout layout = Layout::Compute(800, 600, 300, false);

    EXPECT_EQ(layout.Console.Bottom(), 600);
    EXPECT_EQ(layout.Console.Y, layout.Splitter.Bottom());
    EXPECT_EQ(layout.Splitter.Y, layout.MapFrame.Bottom());
}

TEST(UnstretchedViewNeverExceedsTheMap) {
    const Layout layout = Layout::Compute(1920, 1080, 0, true);

    EXPECT_EQ(layout.View.Width, 480);
    EXPECT_EQ(layout.View.Height, 352);
}
