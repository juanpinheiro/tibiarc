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

#include "ui/sidebar/automap_panel.hpp"

namespace trc {
namespace Client {

namespace {

/* The sunken frame around the map, and the map inside of it. */
constexpr Rect FrameArea{8, 4, 108, 108};
constexpr Rect ViewArea{9, 5, 106, 106};

/* How many pixels two fields take up. The client starts out at 2, a pixel
 * per field, and zooms by doubling or halving it. */
constexpr int Zoom = 2;

constexpr int CompassSkin = 65;
constexpr int CompassX = 124, CompassY = 5;

const ButtonSkins ZoomOutButton{68, 69};
const ButtonSkins ZoomInButton{66, 67};
const ButtonSkins FloorUpButton{70, 71};
const ButtonSkins FloorDownButton{72, 73};

/* The buttons below the compass, two columns by two rows, and the wide one
 * under them. */
constexpr int LeftColumn = 124, RightColumn = 147;
constexpr int UpperRow = 50, LowerRow = 71;
constexpr int CentreX = 124, CentreY = 92;

const Pixel CrossColor(255, 255, 255);

void DrawFrame(Canvas &canvas, const Skin &skin, int x, int y) {
    const Rect frame = FrameArea.Translate(x, y);

    skin.Tile(canvas, Skin::LineTop, Rect{frame.X, frame.Y, frame.Width, 1});
    skin.Tile(canvas, Skin::LineLeft, Rect{frame.X, frame.Y, 1, frame.Height});
    skin.Tile(canvas,
              Skin::LineRight,
              Rect{frame.Right() - 1, frame.Y, 1, frame.Height});
    skin.Tile(canvas,
              Skin::LineBottom,
              Rect{frame.X, frame.Bottom() - 1, frame.Width, 1});
}

void DrawView(Canvas &canvas,
              const Rect &view,
              const Automap &chart,
              const Position &player) {
    const ClipScope clip(canvas, view);

    /* The field the top-left pixel shows, in pixels of the zoomed map. */
    const int left = Zoom * player.X + Zoom / 2 - view.Width;
    const int top = Zoom * player.Y + Zoom / 2 - view.Height;

    for (int row = 0; row < view.Height; row++) {
        for (int column = 0; column < view.Width; column++) {
            const uint8_t color = chart.ColorAt((left + column * 2) / Zoom,
                                                (top + row * 2) / Zoom,
                                                player.Z);

            canvas.Fill(Rect{view.X + column, view.Y + row, 1, 1},
                        Pixel::ProtocolColor(color));
        }
    }

    /* The player is marked by a small cross in the middle. */
    const int centerX = view.X + view.Width / 2;
    const int centerY = view.Y + view.Height / 2;

    canvas.Fill(Rect{centerX - 1, centerY, 3, 1}, CrossColor);
    canvas.Fill(Rect{centerX, centerY - 1, 1, 3}, CrossColor);
}

} // namespace

void DrawAutomapPanel(Canvas &canvas,
                      const Widgets &widgets,
                      int x,
                      int y,
                      const Automap &chart,
                      const Position &player) {
    DrawFrame(canvas, widgets.Pieces, x, y);

    widgets.Pieces.Draw(canvas, CompassSkin, x + CompassX, y + CompassY);
    widgets.Button(canvas, ZoomOutButton, x + LeftColumn, y + UpperRow);
    widgets.Button(canvas, ZoomInButton, x + LeftColumn, y + LowerRow);
    widgets.Button(canvas, FloorUpButton, x + RightColumn, y + UpperRow);
    widgets.Button(canvas, FloorDownButton, x + RightColumn, y + LowerRow);
    widgets.TextButton(canvas,
                       ButtonSkins::Regular,
                       x + CentreX,
                       y + CentreY,
                       "Centre");

    DrawView(canvas, ViewArea.Translate(x, y), chart, player);
}

} // namespace Client
} // namespace trc
