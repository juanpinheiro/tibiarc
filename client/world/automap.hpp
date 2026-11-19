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

#ifndef __TRC_CLIENT_WORLD_AUTOMAP_HPP__
#define __TRC_CLIENT_WORLD_AUTOMAP_HPP__

#include "common/position.hpp"
#include "game/gamestate.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace trc {
namespace Client {

/* The map the client draws up as the player travels: one colour per field,
 * taken from the last object on it that shows on the map. Only the floor
 * the player is on is charted; everything else stays as it was last seen,
 * and fields never seen are black. */
class Automap {
public:
    void Clear();

    /* Charts a field that has just changed. */
    void Survey(const Gamestate &gamestate, const Position &position);

    /* Follows the player: arriving on another floor charts everything in
     * view on it. */
    void Follow(const Gamestate &gamestate);

    /* The colour of a field, as an index into the 6x6x6 colour cube. */
    uint8_t ColorAt(int x, int y, int z) const;

private:
    /* The map is kept in square sectors of this many fields a side. */
    static constexpr int SectorSize = 256;
    static constexpr int NoFloor = -1;

    using Sector = std::array<uint8_t, SectorSize * SectorSize>;

    static uint32_t SectorKey(int x, int y, int z) {
        return (static_cast<uint32_t>(z) << 16) |
               (static_cast<uint32_t>(x / SectorSize) << 8) |
               static_cast<uint32_t>(y / SectorSize);
    }

    static size_t FieldIndex(int x, int y) {
        return static_cast<size_t>(x % SectorSize) * SectorSize +
               (y % SectorSize);
    }

    std::unordered_map<uint32_t, std::unique_ptr<Sector>> Sectors;
    int Floor = NoFloor;
};

} // namespace Client
} // namespace trc

#endif
