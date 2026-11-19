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

#ifndef __TRC_CLIENT_PLAYBACK_SESSION_HPP__
#define __TRC_CLIENT_PLAYBACK_SESSION_HPP__

#include "assets/versions.hpp"
#include "game/gamestate.hpp"
#include "recording/recordings.hpp"
#include "world/options.hpp"
#include "world/world.hpp"

#include <cstdint>
#include <memory>

namespace trc {
namespace Client {

/* A recording being played back: owns the game state and moves it to any
 * point in time. Time is in milliseconds since the start of the recording. */
class Session {
public:
    Session(const Version &version,
            std::unique_ptr<Recordings::Recording> recording);

    uint32_t Runtime() const {
        return Length;
    }

    uint32_t Now() const {
        return Time;
    }

    /* Moves to the given time. Going forwards applies what happened in
     * between; going backwards replays the recording from its start. */
    void Seek(uint32_t time);

    const Gamestate &State() const {
        return Game;
    }

    const World &Surroundings() const {
        return Memory;
    }

    Options Preferences;

private:
    using FrameIterator = std::list<Recordings::Recording::Frame>::const_iterator;

    void Rewind();

    std::unique_ptr<Recordings::Recording> Source;
    Gamestate Game;
    World Memory;
    FrameIterator Next;
    uint32_t Length;
    uint32_t Time = 0;
};

} // namespace Client
} // namespace trc

#endif
