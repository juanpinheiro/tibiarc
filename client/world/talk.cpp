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

#include "world/talk.hpp"

#include "scene/animation.hpp"

#include <algorithm>

namespace trc {
namespace Client {

TalkKind ClassifyTalk(MessageMode mode) {
    switch (mode) {
    case MessageMode::Say:
    case MessageMode::Spell:
    case MessageMode::PlayerToNPC:
    case MessageMode::NPCStart:
    case MessageMode::NPCContinued:
        return TalkKind::Say;
    case MessageMode::Whisper:
        return TalkKind::Whisper;
    case MessageMode::Yell:
        return TalkKind::Yell;
    case MessageMode::PrivateIn:
        return TalkKind::Private;
    case MessageMode::PrivateOut:
    case MessageMode::PlayerToGM:
    case MessageMode::ConsoleBlue:
        return TalkKind::ConsoleBlue;
    case MessageMode::ChannelYellow:
    case MessageMode::RuleViolationAnswer:
    case MessageMode::RuleViolationContinue:
    case MessageMode::Party:
        return TalkKind::Channel;
    case MessageMode::ChannelWhite:
    case MessageMode::RuleViolationChannel:
    case MessageMode::PartyWhite:
        return TalkKind::ChannelWhite;
    case MessageMode::ChannelOrange:
        return TalkKind::ChannelOrange;
    case MessageMode::ChannelRed:
        return TalkKind::ChannelRed;
    case MessageMode::ChannelAnonymousRed:
        return TalkKind::ChannelAnonymous;
    case MessageMode::Broadcast:
        return TalkKind::Broadcast;
    case MessageMode::GMToPlayer:
        return TalkKind::Gamemaster;
    case MessageMode::MonsterSay:
        return TalkKind::MonsterSay;
    case MessageMode::MonsterYell:
        return TalkKind::MonsterYell;
    case MessageMode::Warning:
    case MessageMode::Admin:
        return TalkKind::Admin;
    case MessageMode::Game:
    case MessageMode::Tutorial:
        return TalkKind::Event;
    case MessageMode::Login:
        return TalkKind::Login;
    case MessageMode::Look:
    case MessageMode::Loot:
    case MessageMode::NPCTrade:
    case MessageMode::Guild:
    case MessageMode::Hotkey:
    case MessageMode::Report:
    case MessageMode::ThankYou:
    case MessageMode::Market:
        return TalkKind::Info;
    case MessageMode::Failure:
        return TalkKind::Failure;
    case MessageMode::ConsoleRed:
        return TalkKind::ConsoleRed;
    case MessageMode::ConsoleOrange:
        return TalkKind::ConsoleOrange;
    default:
        return TalkKind::Status;
    }
}

Pixel TalkColor(TalkKind kind) {
    switch (kind) {
    case TalkKind::Say:
    case TalkKind::Whisper:
    case TalkKind::Yell:
    case TalkKind::Channel:
        return Pixel(240, 240, 0);
    case TalkKind::Private:
        return Pixel(96, 248, 248);
    case TalkKind::ChannelOrange:
        return Pixel(240, 180, 0);
    case TalkKind::ChannelRed:
    case TalkKind::ChannelAnonymous:
    case TalkKind::Broadcast:
    case TalkKind::Gamemaster:
    case TalkKind::Admin:
    case TalkKind::ConsoleRed:
        return Pixel(248, 96, 96);
    case TalkKind::MonsterSay:
    case TalkKind::MonsterYell:
    case TalkKind::ConsoleOrange:
        return Pixel(255, 102, 0);
    case TalkKind::Info:
        return Pixel(0, 240, 0);
    case TalkKind::ConsoleBlue:
        return Pixel(160, 160, 255);
    default:
        return Pixel(240, 240, 240);
    }
}

bool IsServerMessage(TalkKind kind) {
    switch (kind) {
    case TalkKind::Admin:
    case TalkKind::Event:
    case TalkKind::Login:
    case TalkKind::Status:
    case TalkKind::Info:
        return true;
    default:
        return false;
    }
}

/* ------------------------------------------------------------------------ */

static bool IsSpace(char character) {
    return character == ' ' || (character >= '\t' && character <= '\r');
}

/* Breaks text into lines by character count, at whitespace where there is
 * any and with a hyphen where there isn't. Whatever doesn't fit in the box
 * is dropped. */
static void BreakIntoLines(std::string_view text,
                           std::vector<std::string> &lines) {
    static constexpr size_t Limit = Talkbox::MaxLineLength;

    while (lines.size() < Talkbox::MaxLines && !text.empty()) {
        size_t space = std::string_view::npos, length = 0;
        bool broken = false;

        for (; length <= Limit; length++) {
            if (length == text.size()) {
                lines.emplace_back(text);
                return;
            }

            if (text[length] == '\n') {
                lines.emplace_back(text.substr(0, length));
                text.remove_prefix(length + 1);
                broken = true;
                break;
            }

            if (IsSpace(text[length])) {
                space = length;
            }
        }

        if (broken) {
            continue;
        }

        if (space != std::string_view::npos) {
            lines.emplace_back(text.substr(0, space));
            text.remove_prefix(space + 1);
        } else {
            lines.emplace_back(std::string(text.substr(0, Limit - 1)) + '-');
            text.remove_prefix(Limit - 1);
        }
    }
}

TalkBoard::Queue TalkBoard::QueueOf(TalkKind kind) {
    switch (kind) {
    case TalkKind::Say:
    case TalkKind::Whisper:
    case TalkKind::Yell:
        return Queue::Speaker;
    case TalkKind::MonsterSay:
    case TalkKind::MonsterYell:
        return Queue::Monster;
    case TalkKind::Broadcast:
    case TalkKind::Gamemaster:
    case TalkKind::Admin:
    case TalkKind::Event:
        return Queue::Announcement;
    case TalkKind::Private:
        return Queue::Private;
    default:
        return Queue::None;
    }
}

static bool SamePlace(const Talkbox &a, const Talkbox &b) {
    return a.Position.X == b.Position.X && a.Position.Y == b.Position.Y &&
           a.Position.Z == b.Position.Z;
}

bool TalkBoard::SameQueue(const Talkbox &a, const Talkbox &b) {
    const Queue queue = QueueOf(a.Kind);

    if (queue != QueueOf(b.Kind)) {
        return false;
    }

    switch (queue) {
    case Queue::Speaker:
        return a.Kind == b.Kind && SamePlace(a, b) &&
               a.Lines.front() == b.Lines.front();
    case Queue::Monster:
        return SamePlace(a, b);
    case Queue::Announcement:
    case Queue::Private:
        return true;
    default:
        return false;
    }
}

void TalkBoard::Clear() {
    Entries.clear();
    StatusActive = false;
    Changes++;
}

Talkbox &TalkBoard::Add(uint32_t now, TalkKind kind, const std::string &text) {
    /* Make room by forgetting the oldest. */
    while (Entries.size() >= MaxBoxes) {
        const auto oldest = std::min_element(
                Entries.begin(),
                Entries.end(),
                [](const Talkbox &a, const Talkbox &b) {
                    return a.Order < b.Order;
                });

        Remove(static_cast<size_t>(oldest - Entries.begin()));
    }

    Talkbox &box = Entries.emplace_back();

    box.Kind = kind;
    box.Order = NextOrder++;
    box.EndTime = now + static_cast<uint32_t>(text.size() / 2 + 30) * 100;
    box.Shown = true;
    box.Positioned = false;
    box.Position = trc::Position(0, 0, 0);
    box.HasHeader = false;

    Changes++;

    return box;
}

void TalkBoard::WaitBehind(Talkbox &box, uint32_t now) {
    uint32_t longest = 0;
    bool waiting = false;

    for (const Talkbox &other : Entries) {
        if (&other != &box && SameQueue(box, other)) {
            longest = std::max(longest,
                               other.EndTime > now ? other.EndTime - now : 0);
            waiting = true;
        }
    }

    if (waiting) {
        box.Shown = false;
        box.EndTime = now + (box.EndTime - now) / 2 + longest;
    }
}

void TalkBoard::Speak(uint32_t now,
                      TalkKind kind,
                      const std::string &speaker,
                      const std::string &text,
                      const trc::Position &position) {
    Talkbox &box = Add(now, kind, text);
    const Queue queue = QueueOf(kind);

    switch (kind) {
    case TalkKind::Say:
        box.Lines.push_back(speaker + " says:");
        break;
    case TalkKind::Whisper:
        box.Lines.push_back(speaker + " whispers:");
        break;
    case TalkKind::Yell:
        box.Lines.push_back(speaker + " yells:");
        break;
    case TalkKind::Private:
    case TalkKind::Broadcast:
    case TalkKind::Gamemaster:
        box.Lines.push_back(speaker + ":");
        break;
    default:
        break;
    }

    box.HasHeader = !box.Lines.empty();

    if (box.HasHeader) {
        box.Lines.front().resize(
                std::min(box.Lines.front().size(), Talkbox::MaxLineLength));
    }

    BreakIntoLines(text, box.Lines);

    if (queue == Queue::Speaker || queue == Queue::Monster) {
        box.Positioned = true;
        box.Position = position;

        /* Whoever is already talking there gets the new lines added to what
         * they're saying, and this box takes over once that one is gone. */
        for (Talkbox &other : Entries) {
            if (&other == &box || !SameQueue(box, other)) {
                continue;
            }

            for (size_t line = box.HasHeader ? 1 : 0;
                 line < box.Lines.size() &&
                 other.Lines.size() < Talkbox::MaxLines;
                 line++) {
                other.Lines.push_back(box.Lines[line]);
            }

            box.Shown = false;
        }
    } else {
        WaitBehind(box, now);
    }
}

void TalkBoard::Announce(uint32_t now, TalkKind kind, const std::string &text) {
    if (kind == TalkKind::Info) {
        /* There's only ever one of these. */
        for (size_t i = Entries.size(); i-- > 0;) {
            if (Entries[i].Kind == TalkKind::Info) {
                Entries.erase(Entries.begin() + i);
            }
        }
    }

    Talkbox &box = Add(now, kind, text);

    BreakIntoLines(text, box.Lines);

    if (kind != TalkKind::Info) {
        box.EndTime = now + (box.EndTime - now) * 2;
        WaitBehind(box, now);
    }
}

void TalkBoard::Remove(size_t index) {
    const Talkbox removed = std::move(Entries[index]);

    Entries.erase(Entries.begin() + index);
    Changes++;

    /* The oldest of those that waited behind it takes its place. */
    Talkbox *next = nullptr;

    for (Talkbox &other : Entries) {
        if (!other.Shown && SameQueue(removed, other) &&
            (next == nullptr || other.Order < next->Order)) {
            next = &other;
        }
    }

    if (next != nullptr) {
        next->Shown = true;
    }
}

void TalkBoard::SetStatus(uint32_t now, const std::string &text) {
    StatusText = text;
    StatusSince = now;
    StatusActive = true;
    Changes++;
}

void TalkBoard::Advance(uint32_t now) {
    /* Timers are only looked at on the animation timer. */
    const uint32_t time = Clock::Ticks(now) * Clock::TickLength;

    for (;;) {
        const Talkbox *expired = nullptr;

        for (const Talkbox &box : Entries) {
            if (box.EndTime <= time &&
                (expired == nullptr || box.Order < expired->Order)) {
                expired = &box;
            }
        }

        if (expired == nullptr) {
            break;
        }

        Remove(static_cast<size_t>(expired - Entries.data()));
    }

    if (StatusActive && Clock::TicksSince(now, StatusSince) >= StatusTicks) {
        StatusActive = false;
        Changes++;
    }
}

std::optional<std::string_view> TalkBoard::Status(uint32_t now) const {
    if (!StatusActive || Clock::TicksSince(now, StatusSince) >= StatusTicks) {
        return std::nullopt;
    }

    return std::string_view(StatusText);
}

} // namespace Client
} // namespace trc
