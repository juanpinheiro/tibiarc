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

#include "viewer.hpp"

#include "ui/mapframe.hpp"

#include <algorithm>

namespace trc {
namespace Client {

Viewer::Viewer(const Version &version,
               std::unique_ptr<Recordings::Recording> recording,
               int width,
               int height)
    : Playback(version, std::move(recording)),
      Text(version.Pictures),
      Pieces(version.Pictures.Get(PictureIndex::Icons)),
      Overlay(Text, Pieces),
      Console(Text, Pieces),
      Side(version, Text, Pieces),
      MapCanvas(MapFrame::Width, MapFrame::Height) {
    Resize(width, height);
}

void Viewer::Resize(int width, int height) {
    width = std::max(width, MinimumWidth);
    height = std::max(height, MinimumHeight);

    if (!Window || Window->Width != width || Window->Height != height) {
        Window = std::make_unique<Canvas>(width, height);
    }

    Areas = Layout::Compute(width,
                            height,
                            ConsoleHeight,
                            Playback.Preferences.NoStretching);
}

void Viewer::SetConsoleHeight(int height) {
    ConsoleHeight = height;
    Resize(Window->Width, Window->Height);
}

const Canvas &Viewer::Render() {
    Canvas &canvas = *Window;
    const uint32_t now = Playback.Now();
    const Gamestate &state = Playback.State();
    const World &world = Playback.Surroundings();
    const Options &options = Playback.Preferences;
    const Rect view = Areas.View.Translate(Areas.MapFrame.X, Areas.MapFrame.Y);

    canvas.SetClip(canvas.Bounds());

    Spots.Clear();
    Console.Draw(canvas, Areas.Console, world.Console, Spots, now);
    Side.Draw(canvas, Areas.Sidebar, state, world, Spots, now);
    DrawMapFrame(canvas, Pieces, Areas);

    Renderer.Render(state, now, MapCanvas, Frame);

    if (options.NoStretching) {
        /* The middle of the map at its own size. */
        canvas.Copy(MapCanvas,
                    Rect{(MapFrame::Width - view.Width) / 2,
                         (MapFrame::Height - view.Height) / 2,
                         view.Width,
                         view.Height},
                    view.X,
                    view.Y);
    } else {
        canvas.Stretch(MapCanvas, MapCanvas.Bounds(), view);
    }

    Overlay.Draw(canvas, view, Frame, state, world, options, now);
    DrawSplitter(canvas, Pieces, Areas);

    return canvas;
}

bool Viewer::Click(int x, int y) {
    const auto spot = Spots.At(x, y);
    Options &options = Playback.Preferences;

    if (!spot) {
        return false;
    }

    switch (spot->What) {
    case Gesture::ToggleSkills:
        options.ShowSkillsWindow = !options.ShowSkillsWindow;
        break;
    case Gesture::ToggleBattle:
        options.ShowBattleWindow = !options.ShowBattleWindow;
        break;
    case Gesture::ToggleVIP:
        options.ShowVIPWindow = !options.ShowVIPWindow;
        break;
    case Gesture::DismissContainer:
        Side.DismissContainer(spot->Target);
        break;
    case Gesture::SelectChannel:
        Console.SelectChannel(Playback.Surroundings().Console, spot->Target);
        break;
    }

    return true;
}

} // namespace Client
} // namespace trc
