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

#include "viewer.hpp"

#include <algorithm>

namespace trc {
namespace Client {

Viewer::Viewer(const Version &version,
               std::unique_ptr<Recordings::Recording> recording,
               int width,
               int height)
    : Playback(version, std::move(recording)),
      Data(version),
      Text(version.Pictures),
      Pieces(version.Pictures.Get(PictureIndex::Icons)),
      Overlay(Text, Pieces),
      MapCanvas(MapFrame::Width, MapFrame::Height) {
    Resize(width, height);
}

void Viewer::Resize(int width, int height) {
    width = std::max(width, MinimumWidth);
    height = std::max(height, MinimumHeight);

    if (!Window || Window->Width != width || Window->Height != height) {
        Window = std::make_unique<Canvas>(width, height);
    }
}

const Canvas &Viewer::Render() {
    Canvas &canvas = *Window;
    const uint32_t now = Playback.Now();
    const Rect view = canvas.Bounds();

    canvas.SetClip(canvas.Bounds());
    canvas.Fill(canvas.Bounds(), Pixel(0, 0, 0));

    Renderer.Render(Playback.State(), now, MapCanvas, Frame);
    canvas.Stretch(MapCanvas, MapCanvas.Bounds(), view);
    Overlay.Draw(canvas,
                 view,
                 Frame,
                 Playback.State(),
                 Playback.Surroundings(),
                 Playback.Preferences,
                 now);

    return canvas;
}

} // namespace Client
} // namespace trc
