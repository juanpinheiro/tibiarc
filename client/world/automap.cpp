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

#include "world/automap.hpp"

#include "assets/versions.hpp"

namespace trc {
namespace Client {

namespace {

/* The fields in view are 18 by 14 with the player on (8, 6). */
constexpr int ViewLeft = 8, ViewTop = 6;

/* What a field looks like from above: the colour of its topmost object that
 * has one, black if there is none. */
uint8_t FieldColor(const Version &version, const Tile &tile) {
    uint8_t color = 0;

    for (int i = 0; i < tile.ObjectCount; i++) {
        const Object &object = tile.Objects[i];

        if (object.IsCreature()) {
            continue;
        }

        const auto &properties = version.GetItem(object.Id).Properties;

        if (properties.Automap) {
            color = static_cast<uint8_t>(properties.AutomapColor);
        }
    }

    return color;
}

} // namespace

void Automap::Clear() {
    Sectors.clear();
    Floor = NoFloor;
}

void Automap::Survey(const Gamestate &gamestate, const Position &position) {
    if (position.Z != gamestate.Map.Position.Z) {
        return;
    }

    auto &sector = Sectors[SectorKey(position.X, position.Y, position.Z)];

    if (!sector) {
        sector = std::make_unique<Sector>();
        sector->fill(0);
    }

    (*sector)[FieldIndex(position.X, position.Y)] =
            FieldColor(gamestate.Version, gamestate.Map.Tile(position));
}

void Automap::Follow(const Gamestate &gamestate) {
    const Position &center = gamestate.Map.Position;

    if (center.Z == Floor) {
        return;
    }

    Floor = center.Z;

    for (int column = 0; column < Map::TileBufferWidth; column++) {
        for (int row = 0; row < Map::TileBufferHeight; row++) {
            const int x = center.X - ViewLeft + column;
            const int y = center.Y - ViewTop + row;

            if (x >= 0 && y >= 0 && x <= UINT16_MAX && y <= UINT16_MAX) {
                Survey(gamestate,
                       Position(static_cast<uint16_t>(x),
                                static_cast<uint16_t>(y),
                                center.Z));
            }
        }
    }
}

uint8_t Automap::ColorAt(int x, int y, int z) const {
    if (x < 0 || y < 0 || x > UINT16_MAX || y > UINT16_MAX) {
        return 0;
    }

    const auto it = Sectors.find(SectorKey(x, y, z));

    return it != Sectors.end() ? (*it->second)[FieldIndex(x, y)] : 0;
}

} // namespace Client
} // namespace trc
