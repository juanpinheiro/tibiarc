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

#include "ui/sidebar/condition_strip.hpp"

#include <utility>
#include <vector>

namespace trc {
namespace Client {

namespace {

constexpr int StripSkin = 100;

/* Where the first picture goes, how far apart they are, and how many the
 * strip has room for. */
constexpr int FirstX = 2, FirstY = 2;
constexpr int Pitch = 10;
constexpr size_t Capacity = 10;

constexpr int PoisonedSkin = 228;
constexpr int BurningSkin = 229;
constexpr int ElectrifiedSkin = 230;
constexpr int DrunkSkin = 231;
constexpr int MagicShieldSkin = 232;
constexpr int ParalysedSkin = 233;
constexpr int HastedSkin = 234;
constexpr int InBattleSkin = 235;

/* The player's own skull; the pictures follow each other in the order of
 * the skulls the client knows. */
constexpr int FirstSkullSkin = 236;
constexpr CharacterSkull LastKnownSkull = CharacterSkull::Red;

bool Suffers(const Gamestate &gamestate, StatusIcon condition) {
    return std::to_underlying(gamestate.Player.Icons & condition) != 0;
}

/* The pictures to show, in the client's fixed order. */
std::vector<int> ActiveConditions(const Gamestate &gamestate) {
    std::vector<int> skins;

    const auto add = [&](StatusIcon condition, int skin) {
        if (Suffers(gamestate, condition)) {
            skins.push_back(skin);
        }
    };

    add(StatusIcon::Poison, PoisonedSkin);
    add(StatusIcon::Burn, BurningSkin);
    add(StatusIcon::ManaShield, MagicShieldSkin);
    add(StatusIcon::Paralyze, ParalysedSkin);

    if (const Creature *self = gamestate.FindCreature(gamestate.Player.Id)) {
        if (self->Skull != CharacterSkull::None &&
            self->Skull <= LastKnownSkull) {
            skins.push_back(FirstSkullSkin + std::to_underlying(self->Skull) -
                            1);
        }
    }

    add(StatusIcon::Swords, InBattleSkin);
    add(StatusIcon::Drunk, DrunkSkin);
    add(StatusIcon::Energy, ElectrifiedSkin);
    add(StatusIcon::Haste, HastedSkin);

    return skins;
}

} // namespace

void DrawConditionStrip(Canvas &canvas,
                        const Widgets &widgets,
                        int x,
                        int y,
                        const Gamestate &gamestate) {
    const std::vector<int> skins = ActiveConditions(gamestate);

    widgets.Pieces.Draw(canvas, StripSkin, x, y);

    for (size_t i = 0; i < skins.size() && i < Capacity; i++) {
        widgets.Pieces.Draw(canvas,
                            skins[i],
                            x + FirstX + Pitch * static_cast<int>(i),
                            y + FirstY);
    }
}

} // namespace Client
} // namespace trc
