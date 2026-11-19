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

#include "world/world.hpp"

namespace trc {
namespace Client {

void World::Reset() {
    Talk.Clear();
    Console.Clear();
    Buddies.clear();
}

void World::Apply(const Events::Base &base, const Gamestate &gamestate) {
    const uint32_t now = gamestate.CurrentTick;

    switch (base.Kind()) {
    case Events::Type::WorldInitialized:
        /* Logging in anew starts from a clean slate. */
        Talk.Clear();
        Buddies.clear();
        break;
    case Events::Type::CreatureSpoke:
    case Events::Type::CreatureSpokeOnMap: {
        const auto &event = static_cast<const Events::CreatureSpoke &>(base);
        const TalkKind kind = ClassifyTalk(event.Mode);

        switch (kind) {
        case TalkKind::Say:
        case TalkKind::Whisper:
        case TalkKind::Yell:
        case TalkKind::MonsterSay:
        case TalkKind::MonsterYell:
            if (base.Kind() == Events::Type::CreatureSpokeOnMap) {
                const auto &placed =
                        static_cast<const Events::CreatureSpokeOnMap &>(base);

                Talk.Speak(now,
                           kind,
                           event.AuthorName,
                           event.Message,
                           placed.Position);
            }
            break;
        case TalkKind::Private:
            if (Preferences.ShowPrivateMessages) {
                Talk.Speak(now,
                           kind,
                           event.AuthorName,
                           event.Message,
                           Position());
            }
            break;
        case TalkKind::Broadcast:
        case TalkKind::Gamemaster:
            Talk.Speak(now,
                       kind,
                       event.AuthorName,
                       event.Message,
                       Position());
            break;
        default:
            break;
        }

        /* Creatures other than players aren't worth logging. */
        if (kind != TalkKind::MonsterSay && kind != TalkKind::MonsterYell) {
            Console.AddSpeech(now, kind, event.AuthorName, event.Message);
        }
        break;
    }
    case Events::Type::CreatureSpokeInChannel: {
        const auto &event =
                static_cast<const Events::CreatureSpokeInChannel &>(base);

        Console.AddChannelSpeech(now,
                                 ClassifyTalk(event.Mode),
                                 event.ChannelId,
                                 event.AuthorName,
                                 event.Message);
        break;
    }
    case Events::Type::StatusMessageReceived:
    case Events::Type::StatusMessageReceivedInChannel: {
        const auto &event =
                static_cast<const Events::StatusMessageReceived &>(base);
        const TalkKind kind = ClassifyTalk(event.Mode);
        bool logged = true;

        switch (kind) {
        case TalkKind::Admin:
            Talk.Announce(now, kind, event.Message);
            break;
        case TalkKind::Event:
            Talk.Announce(now, kind, event.Message);
            logged = Preferences.ShowEventMessages;
            break;
        case TalkKind::Info:
            Talk.Announce(now, kind, event.Message);
            logged = Preferences.ShowInfoMessages;
            break;
        case TalkKind::Login:
            Talk.SetStatus(now, event.Message);
            logged = Preferences.ShowEventMessages;
            break;
        case TalkKind::Status:
            Talk.SetStatus(now, event.Message);
            logged = Preferences.ShowStatusMessages;
            break;
        case TalkKind::Failure:
            Talk.SetStatus(now, event.Message);
            logged = false;
            break;
        default:
            break;
        }

        if (logged) {
            if (IsServerMessage(kind)) {
                Console.AddServerMessage(now, kind, event.Message);
            } else {
                Console.AddSpeech(now, kind, std::string(), event.Message);
            }
        }
        break;
    }
    case Events::Type::ChannelOpened: {
        const auto &event = static_cast<const Events::ChannelOpened &>(base);

        Console.OpenChannel(event.Id, event.Name);
        break;
    }
    case Events::Type::ChannelClosed: {
        const auto &event = static_cast<const Events::ChannelClosed &>(base);

        Console.CloseChannel(event.Id);
        break;
    }
    case Events::Type::PrivateConversationOpened: {
        const auto &event =
                static_cast<const Events::PrivateConversationOpened &>(base);

        Console.OpenPrivate(event.Name);
        break;
    }
    case Events::Type::VIPStatus: {
        const auto &event = static_cast<const Events::VIPStatus &>(base);

        Buddies[event.Id] = Buddy{event.Name, event.Online};
        break;
    }
    case Events::Type::VIPOnlineChanged: {
        const auto &event =
                static_cast<const Events::VIPOnlineChanged &>(base);

        if (auto it = Buddies.find(event.Id); it != Buddies.end()) {
            it->second.Online = event.Online;

            if (event.Online) {
                Talk.SetStatus(now, it->second.Name + " has logged in.");
            }
        }
        break;
    }
    default:
        break;
    }
}

} // namespace Client
} // namespace trc
