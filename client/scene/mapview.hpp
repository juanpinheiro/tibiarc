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

#ifndef __TRC_CLIENT_SCENE_MAPVIEW_HPP__
#define __TRC_CLIENT_SCENE_MAPVIEW_HPP__

#include "game/gamestate.hpp"
#include "graphics/canvas.hpp"

#include "scene/light.hpp"
#include "scene/outfits.hpp"

#include <array>
#include <cstdint>
#include <vector>

namespace trc {
namespace Client {

/* The visible part of the map in coordinates relative to the view: fields
 * are 18 by 14 with the player on (8, 6), and levels count upwards from the
 * lowest floor that is drawn. Fields of different levels that share a
 * coordinate cover each other on screen. */
class MapWindow {
public:
    static constexpr int Columns = LightMap::Columns;
    static constexpr int Rows = LightMap::Rows;
    static constexpr int Levels = 8;

    explicit MapWindow(const Gamestate &gamestate);

    /* The level the player is on. */
    int PlayerLevel() const {
        return Floor;
    }

    bool Underground() const {
        return Center.Z > 7;
    }

    static bool Contains(int column, int row) {
        return column >= 0 && row >= 0 && column < Columns && row < Rows;
    }

    /* The tile of a field, or null if there's nothing there. */
    const Tile *Field(int column, int row, int level) const;

    Position Absolute(int column, int row, int level) const;

    /* Translates a map position into view coordinates, returning false if
     * its floor is not part of the view. */
    bool Relative(const Position &position,
                  int &column,
                  int &row,
                  int &level) const;

private:
    const Gamestate &State;
    Position Center;
    int Floor;
};

/* What a frame of the map leaves behind for the things that are drawn on
 * top of the scaled picture: creature names and bars, animated texts and
 * speech. */
struct MapFrame {
    static constexpr int Width = 480;
    static constexpr int Height = 352;

    struct CreatureMark {
        uint32_t Id;
        /* Top centre of the creature in map pixels. */
        int X, Y;
        int Column, Row, Level;
    };

    struct TextMark {
        /* Top centre of the text in map pixels. */
        int X, Y;
        int Column, Row, Level;
        uint8_t Color;
        uint32_t Value;
    };

    bool Valid = false;

    std::vector<CreatureMark> Creatures;
    std::vector<TextMark> Texts;

    LightMap Light;

    /* Pixel offset of the player within its field. */
    int PlayerOffsetX = 0, PlayerOffsetY = 0;
    int PlayerLevel = 0;
    int TopLevel = 0;

    /* Highest level at which a ground tile covers each field, -1 if none. */
    std::array<int8_t, MapWindow::Columns * MapWindow::Rows> Roof;

    /* Whether something on a higher floor hides the given field. */
    bool Covered(int column, int row, int level) const {
        return MapWindow::Contains(column, row) &&
               Roof[column * MapWindow::Rows + row] > level;
    }
};

class MapRenderer {
public:
    /* Draws the map as the player sees it at `now` onto `canvas`, which
     * must be MapFrame::Width by MapFrame::Height. */
    void Render(const Gamestate &gamestate,
                uint32_t now,
                Canvas &canvas,
                MapFrame &frame);

private:
    struct Pass;

    OutfitColorizer Colorizer;
};

} // namespace Client
} // namespace trc

#endif
