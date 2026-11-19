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

#ifndef __TRC_CLIENT_WORLD_TALK_HPP__
#define __TRC_CLIENT_WORLD_TALK_HPP__

#include "common/pixel.hpp"
#include "common/position.hpp"
#include "game/message.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace trc {
namespace Client {

/* How the client treats a piece of text, which is coarser than the modes of
 * the protocol: several protocol modes look and behave the same. */
enum class TalkKind {
    Say,
    Whisper,
    Yell,
    Private,
    Channel,
    ChannelWhite,
    ChannelOrange,
    ChannelRed,
    ChannelAnonymous,
    Broadcast,
    Gamemaster,
    MonsterSay,
    MonsterYell,
    /* Red warning in the middle of the game window. */
    Admin,
    /* White announcement in the middle of the game window. */
    Event,
    /* White line at the bottom of the game window, also logged. */
    Login,
    /* White line at the bottom of the game window, not logged. */
    Status,
    /* Green text just above the middle of the game window. */
    Info,
    /* White line at the bottom of the game window, never logged. */
    Failure,
    ConsoleBlue,
    ConsoleRed,
    ConsoleOrange,
};

TalkKind ClassifyTalk(MessageMode mode);
Pixel TalkColor(TalkKind kind);

/* Whether continuation lines of a console message are indented; server
 * messages are not. */
bool IsServerMessage(TalkKind kind);

struct Talkbox {
    static constexpr size_t MaxLines = 10;
    static constexpr size_t MaxLineLength = 39;

    TalkKind Kind;
    uint32_t Order;
    uint32_t EndTime;
    bool Shown;

    /* Where it was said; boxes that aren't tied to a place are centred in
     * the game window. */
    bool Positioned;
    trc::Position Position;

    bool HasHeader;
    std::vector<std::string> Lines;
};

/* The texts shown over the map: speech near whoever said it, announcements
 * in the middle and a status line at the bottom. At most ten are tracked,
 * and those that would overlap in meaning wait for their turn. */
class TalkBoard {
public:
    static constexpr size_t MaxBoxes = 10;
    static constexpr uint32_t StatusTicks = 50;

    void Clear();

    void Speak(uint32_t now,
               TalkKind kind,
               const std::string &speaker,
               const std::string &text,
               const trc::Position &position);
    void Announce(uint32_t now, TalkKind kind, const std::string &text);
    void SetStatus(uint32_t now, const std::string &text);

    /* Drops whatever has run its course by `now`. */
    void Advance(uint32_t now);

    const std::vector<Talkbox> &Boxes() const {
        return Entries;
    }

    std::optional<std::string_view> Status(uint32_t now) const;

    /* Changes whenever boxes or the status line appear or disappear. */
    uint32_t Revision() const {
        return Changes;
    }

private:
    enum class Queue { None, Speaker, Monster, Announcement, Private };

    static Queue QueueOf(TalkKind kind);
    static bool SameQueue(const Talkbox &a, const Talkbox &b);

    Talkbox &Add(uint32_t now, TalkKind kind, const std::string &text);
    void Remove(size_t index);
    void WaitBehind(Talkbox &box, uint32_t now);

    std::vector<Talkbox> Entries;
    uint32_t NextOrder = 0;
    uint32_t Changes = 0;

    std::string StatusText;
    uint32_t StatusSince = 0;
    bool StatusActive = false;
};

} // namespace Client
} // namespace trc

#endif
