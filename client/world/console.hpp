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

#ifndef __TRC_CLIENT_WORLD_CONSOLE_HPP__
#define __TRC_CLIENT_WORLD_CONSOLE_HPP__

#include "world/talk.hpp"

#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace trc {
namespace Client {

struct ConsoleMessage {
    TalkKind Kind;
    /* As shown, including any "Name: " prefix. */
    std::string Text;
};

/* The text behind one tab of the console. */
struct ConsoleChannel {
    static constexpr size_t Limit = 1000;

    enum class Type { Default, Public, Private };

    Type Kind;
    uint16_t Id;
    std::string Name;
    /* Whether server messages are copied into this channel. */
    bool ServerMessages = false;

    std::deque<ConsoleMessage> Messages = {};
    /* When the newest message arrived. The tab of a channel that isn't in
     * front draws attention to itself from then on. */
    std::optional<uint32_t> UnreadSince = std::nullopt;
    /* Changes whenever a message is added or dropped. */
    uint32_t Revision = 0;

    void Add(uint32_t now, TalkKind kind, std::string text);
};

class ConsoleLog {
public:
    ConsoleLog();

    void Clear();

    ConsoleChannel &Default() {
        return *Channels.front();
    }

    const ConsoleChannel &Default() const {
        return *Channels.front();
    }

    const std::vector<std::unique_ptr<ConsoleChannel>> &All() const {
        return Channels;
    }

    void OpenChannel(uint16_t id, const std::string &name);
    void OpenPrivate(const std::string &name);
    void CloseChannel(uint16_t id);

    /* Something said by someone, in the open or in private. */
    void AddSpeech(uint32_t now,
                   TalkKind kind,
                   const std::string &speaker,
                   const std::string &text);
    void AddChannelSpeech(uint32_t now,
                          TalkKind kind,
                          uint16_t channel,
                          const std::string &speaker,
                          const std::string &text);
    /* A message from the server itself. */
    void AddServerMessage(uint32_t now,
                          TalkKind kind,
                          const std::string &text);

    /* Changes whenever channels are opened or closed. */
    uint32_t Revision() const {
        return Changes;
    }

private:
    std::vector<std::unique_ptr<ConsoleChannel>> Channels;
    uint32_t Changes = 0;
};

} // namespace Client
} // namespace trc

#endif
