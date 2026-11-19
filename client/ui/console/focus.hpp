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

#ifndef __TRC_CLIENT_UI_CONSOLE_FOCUS_HPP__
#define __TRC_CLIENT_UI_CONSOLE_FOCUS_HPP__

#include "world/console.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace trc {
namespace Client {

/* Which channel of the console the viewer is looking at, and how much of
 * the others they have seen. Channels are told by the name on their tab. */
class ConsoleFocus {
public:
    /* The channel in front: the chosen one for as long as it is open, the
     * default channel otherwise. */
    const ConsoleChannel &Front(const ConsoleLog &log) const;

    /* Brings the channel at the given place among the tabs to the front. */
    void Select(const ConsoleLog &log, size_t place);

    /* Notes that everything in a channel has now been seen. */
    void Visit(const ConsoleChannel &channel);

    /* Whether something was added to a channel since it was last seen. */
    bool Unread(const ConsoleChannel &channel) const;

    /* How many lines up from the newest the text of a channel is scrolled.
     * The text view keeps it within what there is to scroll. */
    int &LinesBack(const ConsoleChannel &channel) {
        return Back[channel.Name];
    }

private:
    std::string Chosen;
    /* The revision of each channel when it was last in front. */
    std::unordered_map<std::string, uint32_t> Seen;
    std::unordered_map<std::string, int> Back;
};

} // namespace Client
} // namespace trc

#endif
