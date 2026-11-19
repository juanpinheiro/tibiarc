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

#ifndef __TRC_CLIENT_UI_SIDEBAR_BATTLE_WINDOW_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_BATTLE_WINDOW_HPP__

#include "game/gamestate.hpp"
#include "ui/sidebar/content_window.hpp"
#include "world/battle.hpp"

namespace trc {
namespace Client {

/* The "Battle" window: a row for every creature around the player, with a
 * small picture of it, its name behind the marks it carries, and a bar for
 * its health. */
class BattleWindow : public ContentWindow {
public:
    BattleWindow(const Gamestate &gamestate, const BattleList &list)
        : State(gamestate), List(list) {
    }

    int InitialHeight() const override;

protected:
    std::string_view Title() const override;
    void DrawIcon(Canvas &canvas,
                  Widgets &widgets,
                  int x,
                  int y,
                  uint32_t now) const override;
    int ContentHeight() const override;
    void DrawList(Canvas &canvas,
                  Widgets &widgets,
                  const Rect &area,
                  uint32_t now) const override;

private:
    void DrawRow(Canvas &canvas,
                 Widgets &widgets,
                 int x,
                 int y,
                 const Creature &creature,
                 uint32_t now) const;

    const Gamestate &State;
    const BattleList &List;
};

} // namespace Client
} // namespace trc

#endif
