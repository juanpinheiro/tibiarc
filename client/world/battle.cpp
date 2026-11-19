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

#include "world/battle.hpp"

#include <algorithm>
#include <cstdlib>

namespace trc {
namespace Client {

namespace {

/* How many fields away from the player a listed creature may be. */
constexpr int RangeX = 7, RangeY = 5;

/* Creature ids below this belong to players. */
constexpr uint32_t PlayerIdLimit = 0x40000000;

bool IsListable(const Gamestate &gamestate, const Creature &creature) {
    /* Monsters and NPCs without any looks are invisible. */
    const bool hasLooks = creature.Id < PlayerIdLimit ||
                          creature.Outfit.Id != 0 ||
                          creature.Outfit.Item.Id != 0;

    return creature.Id != gamestate.Player.Id && hasLooks &&
           creature.Health >= 1;
}

/* Everyone listable around the player, field by field. */
std::vector<uint32_t> Surroundings(const Gamestate &gamestate) {
    const Position &center = gamestate.Map.Position;
    std::vector<uint32_t> result;

    for (int y = center.Y - RangeY; y <= center.Y + RangeY; y++) {
        for (int x = center.X - RangeX; x <= center.X + RangeX; x++) {
            if (x < 0 || y < 0) {
                continue;
            }

            const Tile &tile = gamestate.Map.Tile(x, y, center.Z);

            for (int i = 0; i < tile.ObjectCount; i++) {
                const Object &object = tile.Objects[i];

                if (!object.IsCreature()) {
                    continue;
                }

                const Creature *creature =
                        gamestate.FindCreature(object.CreatureId);

                if (creature != nullptr && IsListable(gamestate, *creature)) {
                    result.push_back(creature->Id);
                }
            }
        }
    }

    return result;
}

bool Contains(const std::vector<uint32_t> &ids, uint32_t id) {
    return std::find(ids.begin(), ids.end(), id) != ids.end();
}

} // namespace

bool BattleList::Reaches(const Gamestate &gamestate, const Position &position) {
    const Position &center = gamestate.Map.Position;

    return position.Z == center.Z &&
           std::abs(static_cast<int>(position.X) - center.X) <= RangeX &&
           std::abs(static_cast<int>(position.Y) - center.Y) <= RangeY;
}

void BattleList::Refresh(const Gamestate &gamestate) {
    const std::vector<uint32_t> around = Surroundings(gamestate);

    std::erase_if(Listed, [&](uint32_t id) { return !Contains(around, id); });

    for (uint32_t id : around) {
        if (!Contains(Listed, id)) {
            Listed.push_back(id);
        }
    }
}

} // namespace Client
} // namespace trc
