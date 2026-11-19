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

#include "ui/sidebar/content_window.hpp"

#include "ui/layout.hpp"
#include "ui/scrollbar.hpp"

#include <algorithm>

namespace trc {
namespace Client {

namespace {

constexpr int Width = Layout::SidebarWidth;

/* The pieces of the frame: the title bar with its two ends, the side
 * borders, and the bottom border with its two corners. */
constexpr int TitleLeftSkin = 128;
constexpr int TitleSkin = 132;
constexpr int TitleRightSkin = 129;
constexpr int LeftBorderSkin = 134;
constexpr int RightBorderSkin = 135;
constexpr int BottomLeftSkin = 130;
constexpr int BottomSkin = 133;
constexpr int BottomRightSkin = 131;

constexpr int TitleHeight = 15;
constexpr int Border = 4;

/* What sits on the title bar. The buttons are placed from the right. */
constexpr int IconX = 4, IconY = 2;
constexpr Rect TitleArea{20, 4, Width - 20 - 41, 10};
constexpr int ButtonsY = 2;
constexpr int CloseX = Width - 15;
constexpr int MinimiseX = Width - 28;
constexpr int UpX = Width - 39;

const ButtonSkins CloseButton{136, 137};
const ButtonSkins MinimiseButton{138, 139};
const ButtonSkins UpButton{126, 127};

const TextStyle TitleStyle{4, Pixel(144, 144, 144), Align::Left};

/* The grip for resizing the window lies over the bottom-left corner of the
 * body. */
constexpr int GripSkin = 42;
constexpr int GripX = 3;
constexpr int GripFromBottom = 16;

void DrawFrame(Canvas &canvas, const Skin &skin, int x, int y, int height) {
    const int inner = Width - Border * 2;
    const int sides = height - TitleHeight - Border;
    const int right = x + Width - Border;
    const int bottom = y + height - Border;

    skin.Draw(canvas, TitleLeftSkin, x, y);
    skin.Tile(canvas, TitleSkin, Rect{x + Border, y, inner, TitleHeight});
    skin.Draw(canvas, TitleRightSkin, right, y);
    skin.Tile(canvas,
              LeftBorderSkin,
              Rect{x, y + TitleHeight, Border, sides});
    skin.Tile(canvas,
              RightBorderSkin,
              Rect{right, y + TitleHeight, Border, sides});
    skin.Draw(canvas, BottomLeftSkin, x, bottom);
    skin.Tile(canvas, BottomSkin, Rect{x + Border, bottom, inner, Border});
    skin.Draw(canvas, BottomRightSkin, right, bottom);
}

} // namespace

Rect ContentWindow::Area(int x, int y, int bodyHeight) {
    return Rect{x, y, Width, bodyHeight + BarsHeight};
}

Rect ContentWindow::CloseButtonArea(int x, int y) {
    return Rect{x + CloseX,
                y + ButtonsY,
                Skin::Width(CloseButton.Up),
                Skin::Height(CloseButton.Up)};
}

int ContentWindow::AtLeastMinimum(int height) const {
    return std::max(height, MinimumHeight());
}

void ContentWindow::Draw(Canvas &canvas,
                         Widgets &widgets,
                         Hotspots &hotspots,
                         uint32_t key,
                         int x,
                         int y,
                         int bodyHeight,
                         int &scroll,
                         uint32_t now) const {
    const int height = bodyHeight + BarsHeight;
    const int page = PageHeight(bodyHeight);

    const int furthest = std::max(0, ContentHeight() - page);

    scroll = std::clamp(scroll, 0, furthest);

    const ScrollRange range{0, furthest, scroll, page};
    const Rect bar{x + Border + ListWidth,
                   y + TitleHeight,
                   Widgets::ScrollbarWidth,
                   bodyHeight};

    const Rect list{x + Border, y + TitleHeight, ListWidth, bodyHeight};

    DrawFrame(canvas, widgets.Pieces, x, y, height);

    widgets.Label(canvas, TitleArea.Translate(x, y), TitleStyle, Title());
    DrawIcon(canvas, widgets, x + IconX, y + IconY, now);

    /* The button that leads to the parent reaches a pixel under the one
     * next to it. */
    if (HasUpButton()) {
        widgets.Button(canvas, UpButton, x + UpX, y + ButtonsY);
    }

    widgets.Button(canvas, MinimiseButton, x + MinimiseX, y + ButtonsY);
    widgets.Button(canvas, CloseButton, x + CloseX, y + ButtonsY);

    widgets.Pieces.Tile(canvas, Skin::Background, list);
    DrawScrollbar(canvas, widgets.Pieces, bar, range);
    hotspots.AddScrollbar(ScrollbarSpot{bar,
                                        range,
                                        ScrollStep(),
                                        Gesture::ScrollWindow,
                                        key});

    {
        /* The list is drawn from further up and cut to the body. */
        const ClipScope clip(canvas, list);

        DrawList(canvas,
                 widgets,
                 Rect{list.X, list.Y - scroll, list.Width, list.Height + scroll},
                 now);
    }

    widgets.Pieces.Draw(canvas,
                        GripSkin,
                        x + GripX,
                        y + height - GripFromBottom);
}

} // namespace Client
} // namespace trc
