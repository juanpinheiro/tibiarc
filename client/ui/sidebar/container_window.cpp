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

#include "ui/sidebar/container_window.hpp"

namespace trc {
namespace Client {

namespace {

constexpr int Columns = 4;

/* The grid: where the first cell is, how far apart cells are, and the room
 * left below the last row. */
constexpr int FirstCellX = 7, FirstCellY = 4;
constexpr int CellPitch = 37;
constexpr int GridMargin = 7;

/* A cell is a frame with room for one object inside its border. */
constexpr int CellSkin = 125;
constexpr int CellBorder = 1;
constexpr int ObjectSize = 32;

/* Enough to show one row of cells. */
constexpr int MinimumBodyHeight = 41;

constexpr int IconSize = 12;

/* The height of a grid of `cells` cells. */
int GridHeight(int cells) {
    return GridMargin + (cells + Columns - 1) / Columns * CellPitch;
}

} // namespace

int ContainerWindow::MinimumHeight() const {
    return MinimumBodyHeight;
}

int ContainerWindow::InitialHeight() const {
    return AtLeastMinimum(GridHeight(InitialItems));
}

std::string_view ContainerWindow::Title() const {
    return Contents.Name;
}

void ContainerWindow::DrawIcon(Canvas &canvas,
                               Widgets &widgets,
                               int x,
                               int y,
                               uint32_t now) const {
    const ClipScope clip(canvas, Rect{x, y, IconSize, IconSize});

    widgets.ObjectIcon(canvas, x, y, IconSize, Object(Contents.ItemId), now);
}

bool ContainerWindow::HasUpButton() const {
    return Contents.HasParent != 0;
}

int ContainerWindow::ContentHeight() const {
    return GridHeight(Contents.SlotsPerPage);
}

void ContainerWindow::DrawList(Canvas &canvas,
                               Widgets &widgets,
                               const Rect &area,
                               uint32_t now) const {
    const int cellSize = Skin::Width(CellSkin);

    for (int index = 0; index < Contents.SlotsPerPage; index++) {
        const int x = area.X + FirstCellX + (index % Columns) * CellPitch;
        const int y = area.Y + FirstCellY + (index / Columns) * CellPitch;

        if (y >= area.Bottom()) {
            break;
        }

        widgets.Pieces.Draw(canvas, CellSkin, x, y);

        if (static_cast<size_t>(index) < Contents.Items.size()) {
            /* Whatever sticks out of a cell is cut off. */
            const ClipScope clip(canvas, Rect{x, y, cellSize, cellSize});

            widgets.ObjectIcon(canvas,
                               x + CellBorder,
                               y + CellBorder,
                               ObjectSize,
                               Contents.Items[index],
                               now);
        }
    }
}

} // namespace Client
} // namespace trc
