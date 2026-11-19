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

#include "world/console.hpp"

#include <algorithm>

namespace trc {
namespace Client {

static std::string Trim(const std::string &text) {
    static constexpr char Whitespace[] = " \t\n\v\f\r";

    const size_t first = text.find_first_not_of(Whitespace);

    if (first == std::string::npos) {
        return {};
    }

    return text.substr(first, text.find_last_not_of(Whitespace) - first + 1);
}

static std::string WithSpeaker(const std::string &speaker,
                               const std::string &text) {
    /* Names are cut short in the console. */
    return speaker.substr(0, 29) + ": " + Trim(text);
}

void ConsoleChannel::Add(uint32_t now, TalkKind kind, std::string text) {
    Messages.push_back(ConsoleMessage{kind, std::move(text)});

    if (Messages.size() > Limit) {
        Messages.pop_front();
    }

    UnreadSince = now;
    Revision++;
}

ConsoleLog::ConsoleLog() {
    Clear();
}

void ConsoleLog::Clear() {
    Channels.clear();
    Channels.push_back(std::make_unique<ConsoleChannel>(
            ConsoleChannel{ConsoleChannel::Type::Default, 0, "Default"}));
    Changes++;
}

void ConsoleLog::OpenChannel(uint16_t id, const std::string &name) {
    for (const auto &channel : Channels) {
        if (channel->Kind == ConsoleChannel::Type::Public &&
            channel->Id == id) {
            return;
        }
    }

    Channels.push_back(std::make_unique<ConsoleChannel>(
            ConsoleChannel{ConsoleChannel::Type::Public, id, name}));
    Changes++;
}

void ConsoleLog::OpenPrivate(const std::string &name) {
    for (const auto &channel : Channels) {
        if (channel->Kind == ConsoleChannel::Type::Private &&
            channel->Name == name) {
            return;
        }
    }

    Channels.push_back(std::make_unique<ConsoleChannel>(
            ConsoleChannel{ConsoleChannel::Type::Private, 0, name}));
    Changes++;
}

void ConsoleLog::CloseChannel(uint16_t id) {
    const auto removed = std::erase_if(Channels, [id](const auto &channel) {
        return channel->Kind == ConsoleChannel::Type::Public &&
               channel->Id == id;
    });

    Changes += static_cast<uint32_t>(removed);
}

void ConsoleLog::AddSpeech(uint32_t now,
                           TalkKind kind,
                           const std::string &speaker,
                           const std::string &text) {
    if (kind == TalkKind::Private) {
        /* Goes to the conversation with that player when there is one. */
        for (const auto &channel : Channels) {
            if (channel->Kind == ConsoleChannel::Type::Private &&
                channel->Name == speaker) {
                channel->Add(now, kind, WithSpeaker(speaker, text));
                return;
            }
        }
    }

    Default().Add(now,
                  kind,
                  speaker.empty() ? Trim(text) : WithSpeaker(speaker, text));
}

void ConsoleLog::AddChannelSpeech(uint32_t now,
                                  TalkKind kind,
                                  uint16_t id,
                                  const std::string &speaker,
                                  const std::string &text) {
    /* What's said in a channel that isn't open is lost. */
    for (const auto &channel : Channels) {
        if (channel->Kind == ConsoleChannel::Type::Public &&
            channel->Id == id) {
            channel->Add(now,
                         kind,
                         (kind == TalkKind::ChannelAnonymous || speaker.empty())
                                 ? Trim(text)
                                 : WithSpeaker(speaker, text));
            return;
        }
    }
}

void ConsoleLog::AddServerMessage(uint32_t now,
                                  TalkKind kind,
                                  const std::string &text) {
    const std::string trimmed = Trim(text);

    Default().Add(now, kind, trimmed);

    for (const auto &channel : Channels) {
        if (channel->Kind != ConsoleChannel::Type::Default &&
            channel->ServerMessages) {
            channel->Add(now, kind, trimmed);
        }
    }
}

} // namespace Client
} // namespace trc
