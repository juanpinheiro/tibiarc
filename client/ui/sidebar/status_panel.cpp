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

#include "ui/sidebar/status_panel.hpp"

#include <string>

namespace trc {
namespace Client {

namespace {

/* One of the two bars: a small picture, the empty bar, the part of it that
 * is filled, and the figure to its right. */
struct BarLayout {
    int IconSkin;
    int EmptySkin;
    int FilledSkin;
    int IconY;
    int BarY;
    int FigureY;
};

constexpr BarLayout HitPointsBar{119, 120, 121, 5, 4, 5};
constexpr BarLayout ManaBar{122, 123, 124, 18, 17, 18};

constexpr int IconX = 9;
constexpr int BarX = 26;
constexpr int BarWidth = 90;
constexpr int FigureX = 127;

const TextStyle FigureStyle{4, Pixel(176, 176, 176), Align::Left};

void DrawBar(Canvas &canvas,
             const Widgets &widgets,
             int x,
             int y,
             const BarLayout &layout,
             int value,
             int maximum) {
    const int filled = (maximum > 0 && value < maximum)
                               ? BarWidth * value / maximum
                               : BarWidth;

    widgets.Pieces.Draw(canvas, layout.IconSkin, x + IconX, y + layout.IconY);
    widgets.Pieces.Draw(canvas, layout.EmptySkin, x + BarX, y + layout.BarY);
    widgets.Write(canvas,
                  x + FigureX,
                  y + layout.FigureY,
                  FigureStyle,
                  std::to_string(value));

    /* The bar is filled from the left by showing that much of the full
     * one. */
    widgets.Pieces.Tile(canvas,
                        layout.FilledSkin,
                        Rect{x + BarX,
                             y + layout.BarY,
                             filled,
                             Skin::Height(layout.FilledSkin)});
}

} // namespace

void DrawStatusPanel(Canvas &canvas,
                     const Widgets &widgets,
                     int x,
                     int y,
                     const PlayerData &player) {
    const auto &stats = player.Stats;

    DrawBar(canvas, widgets, x, y, HitPointsBar, stats.Health, stats.MaxHealth);
    DrawBar(canvas, widgets, x, y, ManaBar, stats.Mana, stats.MaxMana);
}

} // namespace Client
} // namespace trc
