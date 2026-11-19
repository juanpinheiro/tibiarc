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

#ifndef __TRC_CLIENT_SCENE_OVERLAY_HPP__
#define __TRC_CLIENT_SCENE_OVERLAY_HPP__

#include "resources/fonts.hpp"
#include "resources/skin.hpp"
#include "game/gamestate.hpp"
#include "scene/mapview.hpp"
#include "world/options.hpp"
#include "world/world.hpp"

#include <vector>

namespace trc {
namespace Client {

/* Everything drawn over the map picture after it has been scaled to the game
 * window: creature names and health bars, speech, the status line and
 * animated numbers. These keep their size whatever the size of the window;
 * only their positions follow the map. */
class MapOverlay {
public:
    MapOverlay(const Fonts &fonts, const Skin &skin)
        : Text(fonts), Pieces(skin) {
    }

    /* Draws onto `view`, the rectangle of the canvas that shows the map. */
    void Draw(Canvas &canvas,
              const Rect &view,
              const MapFrame &frame,
              const Gamestate &gamestate,
              const World &world,
              const Options &options,
              uint32_t now);

private:
    struct Placement {
        int X, Y;
        Align Alignment;
        /* Boxes pinned to an edge of the view don't scroll with the map. */
        bool PinnedX, PinnedY;
    };

    void DrawCreatures(Canvas &canvas,
                       const Rect &view,
                       const MapFrame &frame,
                       const Gamestate &gamestate,
                       const Options &options);
    void DrawTalk(Canvas &canvas,
                  const Rect &view,
                  const MapFrame &frame,
                  const Gamestate &gamestate,
                  const World &world,
                  const Options &options,
                  uint32_t now);
    void DrawTexts(Canvas &canvas,
                   const Rect &view,
                   const MapFrame &frame,
                   const Options &options);

    void PlaceTalk(const Rect &view,
                   const Gamestate &gamestate,
                   const World &world,
                   const Options &options,
                   bool statusShown);

    const Fonts &Text;
    const Skin &Pieces;

    /* Speech is only laid out anew when something about it changes. */
    std::vector<Placement> Placements;
    uint32_t PlacedRevision = ~0u;
    Position PlacedAt;
    int PlacedWidth = 0, PlacedHeight = 0;
    bool PlacedStatus = false;
};

} // namespace Client
} // namespace trc

#endif
