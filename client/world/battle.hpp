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

#ifndef __TRC_CLIENT_WORLD_BATTLE_HPP__
#define __TRC_CLIENT_WORLD_BATTLE_HPP__

#include "common/position.hpp"
#include "game/gamestate.hpp"

#include <cstdint>
#include <vector>

namespace trc {
namespace Client {

/* The creatures of the battle list: everyone alive on the player's floor
 * and close enough to be on screen. The client neither sorts them nor
 * starts over: a creature that comes into view joins at the end and stays
 * in its place until it leaves. */
class BattleList {
public:
    void Clear() {
        Listed.clear();
    }

    /* Whether a creature standing on `position` is close enough to be
     * listed. */
    static bool Reaches(const Gamestate &gamestate, const Position &position);

    /* Brings the list up to date with who is around right now. */
    void Refresh(const Gamestate &gamestate);

    /* Creature ids, in the order they are listed. */
    const std::vector<uint32_t> &Creatures() const {
        return Listed;
    }

private:
    std::vector<uint32_t> Listed;
};

} // namespace Client
} // namespace trc

#endif
