/*
 * Copyright 2011-2016 "Silver Squirrel Software Handelsbolag"
 * Copyright 2023-2024 "John Högberg"
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

#ifndef __TRC_MESSAGE_HPP__
#define __TRC_MESSAGE_HPP__

#include <cstdint>


namespace trc {
/* Priority-ordered canonical message modes, lower values have higher display
 * precedence. */

enum class MessageMode {
    PrivateIn = 0,
    PrivateOut,

    Say,
    Whisper,
    Yell,

    ChannelWhite,
    ChannelYellow,
    ChannelOrange,
    ChannelRed,
    ChannelAnonymousRed,

    ConsoleBlue,
    ConsoleOrange,
    ConsoleRed,

    Spell,

    NPCStart,
    NPCContinued,
    PlayerToNPC,

    Broadcast,

    GMToPlayer,
    PlayerToGM,

    Login,
    Admin,
    Game,
    Failure,

    Look,

    DamageDealt,

    DamageReceived,
    Healing,
    Experience,

    DamageReceivedOthers,
    HealingOthers,
    ExperienceOthers,

    Status,
    Loot,
    NPCTrade,
    Guild,

    PartyWhite,

    Party,

    MonsterSay,
    MonsterYell,

    Report,
    Hotkey,

    Tutorial,
    ThankYou,
    Market,

    Mana,

    Warning,

    RuleViolationChannel,
    RuleViolationAnswer,
    RuleViolationContinue,
};

} // namespace trc

#endif /* __TRC_MESSAGE_HPP__ */
