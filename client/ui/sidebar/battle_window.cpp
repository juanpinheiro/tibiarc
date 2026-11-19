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

#include "ui/sidebar/battle_window.hpp"

#include "scene/health.hpp"

#include <utility>

namespace trc {
namespace Client {

namespace {

constexpr int IconSkin = 144;

constexpr int RowHeight = 22;

/* The picture of the creature. */
constexpr int OutfitX = 2, OutfitY = 2;
constexpr int OutfitSize = 18;

/* The marks a creature carries come first on the line of its name, each
 * taking up this much room. */
constexpr int NameX = 23, NameY = 2;
constexpr int NameMaximumWidth = 110;
constexpr int MarkPitch = 13;

constexpr Rect HealthBar{23, 14, 122, 5};

/* The pictures of the party marks and skulls follow each other in the order
 * of the ones the client knows. */
constexpr int FirstShieldSkin = 248;
constexpr PartyShield LastKnownShield = PartyShield::Yellow;
constexpr int FirstSkullSkin = 240;
constexpr CharacterSkull LastKnownSkull = CharacterSkull::Red;

const TextStyle NameStyle{4, Pixel(176, 176, 176), Align::Left};

} // namespace

int BattleWindow::InitialHeight() const {
    return AtLeastMinimum(ContentHeight());
}

std::string_view BattleWindow::Title() const {
    return "Battle";
}

void BattleWindow::DrawIcon(Canvas &canvas,
                            Widgets &widgets,
                            int x,
                            int y,
                            [[maybe_unused]] uint32_t now) const {
    widgets.Pieces.Draw(canvas, IconSkin, x, y);
}

int BattleWindow::ContentHeight() const {
    return static_cast<int>(List.Creatures().size()) * RowHeight + 1;
}

void BattleWindow::DrawRow(Canvas &canvas,
                           Widgets &widgets,
                           int x,
                           int y,
                           const Creature &creature,
                           uint32_t now) const {
    const Creature *self = State.FindCreature(State.Player.Id);
    const bool inParty = self != nullptr && self->Shield != PartyShield::None;
    int nameX = x + NameX;

    widgets.OutfitIcon(canvas,
                       x + OutfitX,
                       y + OutfitY,
                       OutfitSize,
                       creature.Outfit,
                       now);

    /* An invitation to a party is of no interest to someone who is in one
     * already. */
    if (creature.Shield != PartyShield::None &&
        creature.Shield <= LastKnownShield &&
        !(creature.Shield == PartyShield::WhiteYellow && inParty)) {
        widgets.Pieces.Draw(canvas,
                            FirstShieldSkin +
                                    std::to_underlying(creature.Shield) - 1,
                            nameX,
                            y + NameY);
        nameX += MarkPitch;
    }

    if (creature.Skull != CharacterSkull::None &&
        creature.Skull <= LastKnownSkull) {
        widgets.Pieces.Draw(canvas,
                            FirstSkullSkin +
                                    std::to_underlying(creature.Skull) - 1,
                            nameX,
                            y + NameY);
        nameX += MarkPitch;
    }

    widgets.Write(canvas,
                  nameX,
                  y + NameY,
                  NameStyle,
                  widgets.Abbreviate(NameStyle.Font,
                                     creature.Name,
                                     NameMaximumWidth));
    widgets.Gauge(canvas,
                  HealthBar.Translate(x, y),
                  creature.Health,
                  HealthColor(creature.Health));
}

/* A third of an entry. */
int BattleWindow::ScrollStep() const {
    return 7;
}

void BattleWindow::DrawList(Canvas &canvas,
                            Widgets &widgets,
                            const Rect &area,
                            uint32_t now) const {
    int top = area.Y;

    for (uint32_t id : List.Creatures()) {
        if (top >= area.Bottom()) {
            break;
        }

        if (const Creature *creature = State.FindCreature(id)) {
            /* A row keeps to itself, however large the creature. */
            const ClipScope clip(canvas,
                                 Rect{area.X, top, area.Width, RowHeight});

            DrawRow(canvas, widgets, area.X, top, *creature, now);
        }

        top += RowHeight;
    }
}

} // namespace Client
} // namespace trc
