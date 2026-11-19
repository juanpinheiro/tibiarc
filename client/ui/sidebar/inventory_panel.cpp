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

#include "ui/sidebar/inventory_panel.hpp"

#include "assets/versions.hpp"
#include "ui/sidebar/condition_strip.hpp"
#include "ui/sidebar/fight_panel.hpp"

#include <string>
#include <string_view>

namespace trc {
namespace Client {

namespace {

/* Where the slots and the fight buttons go within the panel. */
constexpr int SlotsX = 8, SlotsY = 4;
constexpr int FightX = 124, FightY = 19;

/* A slot is a frame with room for one object inside its border. */
constexpr int SlotFrameSkin = 102;
constexpr int SlotBorder = 1;
constexpr int ObjectSize = 32;

struct SlotLayout {
    InventorySlot Slot;
    /* Top-left corner of the frame. */
    int X, Y;
    /* The silhouette shown while the slot is empty. */
    int EmptySkin;
};

constexpr SlotLayout Slots[] = {
        {InventorySlot::Head, 37, 0, 88},
        {InventorySlot::Amulet, 0, 14, 89},
        {InventorySlot::Backpack, 74, 14, 90},
        {InventorySlot::Chest, 37, 37, 91},
        {InventorySlot::RightArm, 74, 51, 93},
        {InventorySlot::LeftArm, 0, 51, 92},
        {InventorySlot::Legs, 37, 74, 94},
        {InventorySlot::Boots, 37, 111, 95},
        {InventorySlot::Ring, 0, 88, 96},
        {InventorySlot::Quiver, 74, 88, 97},
};

const ButtonSkins MinimiseButton{138, 139};

/* The two figures below the slots, each a caption over a number on a
 * backdrop of their own. */
constexpr int FigureSkin = 98;
constexpr int SoulX = 0, CapacityX = 74, FiguresY = 124;
constexpr int FigureCenter = 17;
constexpr int CaptionY = 2, NumberY = 11;

constexpr int ConditionsY = 147;

const TextStyle CaptionStyle{3, Pixel(255, 255, 255), Align::Center};
const TextStyle NumberStyle{4, Pixel(176, 176, 176), Align::Center};

void DrawFigure(Canvas &canvas,
                const Widgets &widgets,
                int x,
                int y,
                std::string_view caption,
                uint32_t number) {
    widgets.Pieces.Draw(canvas, FigureSkin, x, y);
    widgets.Write(canvas,
                  x + FigureCenter,
                  y + CaptionY,
                  CaptionStyle,
                  caption);
    widgets.Write(canvas,
                  x + FigureCenter,
                  y + NumberY,
                  NumberStyle,
                  std::to_string(number));
}

void DrawSlot(Canvas &canvas,
              Widgets &widgets,
              int x,
              int y,
              const SlotLayout &layout,
              const Object &object,
              uint32_t now) {
    const int objectX = x + layout.X + SlotBorder;
    const int objectY = y + layout.Y + SlotBorder;

    widgets.Pieces.Draw(canvas, SlotFrameSkin, x + layout.X, y + layout.Y);

    if (object.Id == 0) {
        widgets.Pieces.Draw(canvas, layout.EmptySkin, objectX, objectY);
        return;
    }

    const ClipScope clip(canvas,
                         Rect{objectX, objectY, ObjectSize, ObjectSize});

    widgets.ObjectIcon(canvas, objectX, objectY, ObjectSize, object, now);
}

} // namespace

void DrawInventoryPanel(Canvas &canvas,
                        Widgets &widgets,
                        int x,
                        int y,
                        const Gamestate &gamestate,
                        uint32_t now) {
    const PlayerData &player = gamestate.Player;
    const int left = x + SlotsX, top = y + SlotsY;

    DrawFigure(canvas,
               widgets,
               left + CapacityX,
               top + FiguresY,
               "Cap:",
               player.Stats.Capacity /
                       gamestate.Version.Features.CapacityDivisor);
    DrawFigure(canvas,
               widgets,
               left + SoulX,
               top + FiguresY,
               "Soul:",
               player.Stats.SoulPoints);
    DrawConditionStrip(canvas, widgets, left, top + ConditionsY, gamestate);
    widgets.Button(canvas, MinimiseButton, left, top);

    for (const SlotLayout &layout : Slots) {
        DrawSlot(canvas,
                 widgets,
                 left,
                 top,
                 layout,
                 player.Inventory(layout.Slot),
                 now);
    }

    DrawFightPanel(canvas, widgets, x + FightX, y + FightY, player);
}

} // namespace Client
} // namespace trc
