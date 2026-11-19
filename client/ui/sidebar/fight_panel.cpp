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

#include "ui/sidebar/fight_panel.hpp"

#include <string_view>

namespace trc {
namespace Client {

namespace {

/* How to fight, as the server numbers it. */
enum AttackMode { Offensive = 1, Balanced = 2, Defensive = 3 };

const ButtonSkins OffensiveButton{103, 104};
const ButtonSkins BalancedButton{105, 106};
const ButtonSkins DefensiveButton{107, 108};
const ButtonSkins StandButton{109, 110};
const ButtonSkins ChaseButton{111, 112};
const ButtonSkins SafeFightButton{113, 114};

/* The picture buttons stand in two columns of three; the buttons with a
 * caption follow below, as wide as both columns. */
constexpr int FightColumn = 0, ChaseColumn = 23;
constexpr int RowPitch = 20;
constexpr int StopY = 62, OptionsY = 86, HelpY = 108;

/* What the buttons show: the modes the server reported or, as long as it
 * hasn't, the ones the client starts out with. */
struct Tactics {
    int Attack = Balanced;
    bool Chase = false;
    bool Secure = true;

    explicit Tactics(const PlayerData &player) {
        if (player.AttackMode != 0) {
            Attack = player.AttackMode;
            Chase = player.ChaseMode != 0;
            Secure = player.SecureMode != 0;
        }
    }
};

} // namespace

void DrawFightPanel(Canvas &canvas,
                    const Widgets &widgets,
                    int x,
                    int y,
                    const PlayerData &player) {
    const Tactics tactics(player);

    const auto button = [&](const ButtonSkins &skins,
                            int column,
                            int row,
                            bool down) {
        widgets.Button(canvas, skins, x + column, y + row * RowPitch, down);
    };
    const auto textButton = [&](int top, std::string_view caption) {
        widgets.TextButton(canvas, ButtonSkins::Regular, x, y + top, caption);
    };

    button(OffensiveButton, FightColumn, 0, tactics.Attack == Offensive);
    button(BalancedButton, FightColumn, 1, tactics.Attack == Balanced);
    button(DefensiveButton, FightColumn, 2, tactics.Attack == Defensive);

    button(StandButton, ChaseColumn, 0, !tactics.Chase);
    button(ChaseButton, ChaseColumn, 1, tactics.Chase);
    /* This one is down while anyone may be attacked. */
    button(SafeFightButton, ChaseColumn, 2, !tactics.Secure);

    textButton(StopY, "Stop");
    textButton(OptionsY, "Options");
    textButton(HelpY, "Help");
}

} // namespace Client
} // namespace trc
