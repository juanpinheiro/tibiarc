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

#include "playback/session.hpp"

#include <algorithm>

namespace trc {
namespace Client {

Session::Session(const Version &version,
                 std::unique_ptr<Recordings::Recording> recording)
    : Source(std::move(recording)),
      Game(version),
      Memory(Preferences),
      Next(Source->Frames.cbegin()),
      Length(static_cast<uint32_t>(
              std::max<int64_t>(0, Source->Runtime.count()))) {
    Seek(0);
}

void Session::Rewind() {
    Game.Reset();
    Game.CurrentTick = 0;
    Memory.Reset();
    Next = Source->Frames.cbegin();
}

void Session::Seek(uint32_t time) {
    time = std::min(time, Length);

    if (time < Time) {
        Rewind();
    }

    for (; Next != Source->Frames.cend(); Next = std::next(Next)) {
        const auto stamp = static_cast<uint32_t>(
                std::max<int64_t>(0, Next->Timestamp.count()));

        if (stamp > time) {
            break;
        }

        Game.CurrentTick = stamp;

        for (const auto &event : Next->Events) {
            event->Update(Game);
            Memory.Apply(*event, Game);
        }
    }

    Time = time;
    Game.CurrentTick = time;
    Memory.Advance(time);
}

} // namespace Client
} // namespace trc
