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

#include "ui/console/focus.hpp"

namespace trc {
namespace Client {

const ConsoleChannel &ConsoleFocus::Front(const ConsoleLog &log) const {
    for (const auto &channel : log.All()) {
        if (channel->Name == Chosen) {
            return *channel;
        }
    }

    return log.Default();
}

void ConsoleFocus::Select(const ConsoleLog &log, size_t place) {
    if (place < log.All().size()) {
        Chosen = log.All()[place]->Name;
    }
}

void ConsoleFocus::Visit(const ConsoleChannel &channel) {
    Seen[channel.Name] = channel.Revision;
}

bool ConsoleFocus::Unread(const ConsoleChannel &channel) const {
    const auto seen = Seen.find(channel.Name);

    /* A channel never looked at is unread as soon as it holds anything. */
    return seen == Seen.end() ? channel.Revision != 0
                              : seen->second != channel.Revision;
}

} // namespace Client
} // namespace trc
