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

#include "ui/console_panel.hpp"

#include "ui/console/input_line.hpp"
#include "ui/console/tab_strip.hpp"
#include "ui/console/text_view.hpp"

namespace trc {
namespace Client {

/* Skins of the console's own frame and of the buttons on it. */
enum ConsoleSkin : int {
    BarLeft = 177,
    BarRight = 178,
    CornerBottomLeft = 179,
    CornerBottomRight = 180,
    EdgeLeft = 181,
    EdgeRight = 182,
    Bar = 183,
    EdgeBottom = 184,
    Texture = 185,
    IgnoreButton = 192,
    NewChannelButton = 195,
    /* The volume button as it looks when set to plain speech. */
    VolumeButton = 197,
    MoreTabsButton = 205,
};

/* Where the parts go, counted from the left and top of the console or,
 * for those named so, from its right and bottom. */
static constexpr int TabStripX = 18;
static constexpr int TabStripRight = 82;
static constexpr int TextX = 4;
static constexpr int TextY = 21;
static constexpr int TextRight = 4;
static constexpr int TextBottom = 22;
static constexpr int InputX = 23;
static constexpr int InputRight = 4;
static constexpr int InputBottom = 20;
static constexpr int InputHeight = 16;
static constexpr int VolumeButtonX = 5;
static constexpr int VolumeButtonBottom = 20;
static constexpr int MoreTabsButtonRight = 82;
static constexpr int NewChannelButtonRight = 32;
static constexpr int IgnoreButtonRight = 16;

void ConsolePanel::DrawBackground(Canvas &canvas, const Rect &area) const {
    const int bar = Skin::Height(Bar);
    const int edge = Skin::Width(EdgeLeft);
    const int right = area.Right() - edge, bottom = area.Bottom() - edge;
    const int sides = area.Height - bar - edge;

    Pieces.Draw(canvas, BarLeft, area.X, area.Y);
    Pieces.Tile(canvas,
                Bar,
                Rect{area.X + edge, area.Y, area.Width - 2 * edge, bar});
    Pieces.Draw(canvas, BarRight, right, area.Y);
    Pieces.Tile(canvas, EdgeLeft, Rect{area.X, area.Y + bar, edge, sides});
    Pieces.Tile(canvas, EdgeRight, Rect{right, area.Y + bar, edge, sides});
    Pieces.Draw(canvas, CornerBottomLeft, area.X, bottom);
    Pieces.Tile(canvas,
                EdgeBottom,
                Rect{area.X + edge, bottom, area.Width - 2 * edge, edge});
    Pieces.Draw(canvas, CornerBottomRight, right, bottom);

    /* The texture is anchored to the top of the window rather than to the
     * console, so it doesn't move along when the console is resized. */
    Pieces.Tile(canvas,
                Texture,
                Rect{area.X + edge, area.Y + bar, area.Width - 2 * edge, sides},
                0,
                area.Y % Skin::Height(Texture));
}

void ConsolePanel::DrawButtons(Canvas &canvas,
                               const Rect &area,
                               bool moreTabs) const {
    /* Shown only while there are tabs that didn't fit. */
    if (moreTabs) {
        Pieces.Draw(canvas,
                    MoreTabsButton,
                    area.Right() - MoreTabsButtonRight,
                    area.Y);
    }

    Pieces.Draw(canvas,
                NewChannelButton,
                area.Right() - NewChannelButtonRight,
                area.Y);
    Pieces.Draw(canvas,
                IgnoreButton,
                area.Right() - IgnoreButtonRight,
                area.Y);
    Pieces.Draw(canvas,
                VolumeButton,
                area.X + VolumeButtonX,
                area.Bottom() - VolumeButtonBottom);
}

void ConsolePanel::Draw(Canvas &canvas,
                        const Rect &area,
                        const ConsoleLog &log,
                        uint32_t now) {
    ClipScope clip(canvas, area);

    DrawBackground(canvas, area);

    const bool moreTabs =
            DrawTabStrip(canvas,
                         Text,
                         Pieces,
                         Rect{area.X + TabStripX,
                              area.Y,
                              area.Width - TabStripX - TabStripRight,
                              Skin::Height(Bar)},
                         log,
                         now);

    DrawButtons(canvas, area, moreTabs);
    DrawConsoleText(canvas,
                    Text,
                    Pieces,
                    Rect{area.X + TextX,
                         area.Y + TextY,
                         area.Width - TextX - TextRight,
                         area.Height - TextY - TextBottom},
                    log.Default());
    DrawInputLine(canvas,
                  Text,
                  Pieces,
                  Rect{area.X + InputX,
                       area.Bottom() - InputBottom,
                       area.Width - InputX - InputRight,
                       InputHeight},
                  now);
}

} // namespace Client
} // namespace trc
