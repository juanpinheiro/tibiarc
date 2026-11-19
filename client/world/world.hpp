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

#ifndef __TRC_CLIENT_WORLD_WORLD_HPP__
#define __TRC_CLIENT_WORLD_WORLD_HPP__

#include "game/gamestate.hpp"
#include "protocol/events.hpp"

#include "world/automap.hpp"
#include "world/battle.hpp"
#include "world/console.hpp"
#include "world/options.hpp"
#include "world/talk.hpp"

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace trc {
namespace Client {

/* Everything the client remembers on top of the game state proper: what was
 * said, which channels are open, who is on the VIP list, where the player
 * has been. It is fed the same events as the game state, in the same order. */
class World {
public:
    struct Buddy {
        std::string Name;
        bool Online = false;
        /* When the buddy last logged in or out while on the list. */
        std::optional<uint32_t> ChangeTick;
    };

    /* The window of an open container. It keeps the size it was given when
     * it opened, whatever is put into the container later. */
    struct OpenContainer {
        uint32_t Id;
        int InitialItems;
        /* Tells this window from every other one of the session, including
         * later ones of the same container. */
        uint32_t Serial;
    };

    explicit World(const Options &options) : Preferences(options) {
    }

    void Reset();

    /* Applies an event that has just been applied to `gamestate`. */
    void Apply(const Events::Base &event, const Gamestate &gamestate);

    /* Lets time pass, expiring what's on screen. */
    void Advance(uint32_t now) {
        Talk.Advance(now);
    }

    const Options &Settings() const {
        return Preferences;
    }

    TalkBoard Talk;
    ConsoleLog Console;
    std::map<uint32_t, Buddy> Buddies;
    Automap Chart;
    BattleList Battle;
    /* In the order the windows were opened. */
    std::vector<OpenContainer> OpenContainers;
    uint32_t ContainerWindowsOpened = 0;

private:
    void OpenContainerWindow(uint32_t id, const Gamestate &gamestate);
    void CloseContainerWindow(uint32_t id);

    /* Notes that something changed on a field. */
    void Disturb(const Position &position, const Gamestate &gamestate);

    const Options &Preferences;
};

} // namespace Client
} // namespace trc

#endif
