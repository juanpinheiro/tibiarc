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

#include "ui/sidebar.hpp"

#include "ui/layout.hpp"
#include "ui/sidebar/automap_panel.hpp"
#include "ui/sidebar/battle_window.hpp"
#include "ui/sidebar/container_window.hpp"
#include "ui/sidebar/inventory_panel.hpp"
#include "ui/sidebar/skills_window.hpp"
#include "ui/sidebar/status_panel.hpp"
#include "ui/sidebar/vip_window.hpp"

#include <memory>

namespace trc {
namespace Client {

namespace {

constexpr int Width = Layout::SidebarWidth;

/* The frame around the fixed panels. */
constexpr int Border = 2;
constexpr int TopLeftSkin = 26;
constexpr int TopRightSkin = 27;
constexpr int BottomLeftSkin = 28;
constexpr int BottomRightSkin = 29;
constexpr int LeftSkin = 30;
constexpr int RightSkin = 31;
constexpr int TopSkin = 32;
constexpr int BottomSkin = 33;

constexpr int PanelWidth = Width - Border * 2;
constexpr int PanelsHeight = AutomapPanelHeight + StatusPanelHeight +
                             InventoryPanelHeight + ButtonBarHeight;
constexpr int FixedHeight = PanelsHeight + Border * 2;

void DrawFrame(Canvas &canvas, const Skin &skin, int x, int y) {
    const int right = x + Width - Border;
    const int bottom = y + FixedHeight - Border;

    skin.Draw(canvas, TopLeftSkin, x, y);
    skin.Draw(canvas, TopRightSkin, right, y);
    skin.Draw(canvas, BottomLeftSkin, x, bottom);
    skin.Draw(canvas, BottomRightSkin, right, bottom);
    skin.Tile(canvas, TopSkin, Rect{x + Border, y, PanelWidth, Border});
    skin.Tile(canvas, BottomSkin, Rect{x + Border, bottom, PanelWidth, Border});
    skin.Tile(canvas, LeftSkin, Rect{x, y + Border, Border, PanelsHeight});
    skin.Tile(canvas, RightSkin, Rect{right, y + Border, Border, PanelsHeight});
}

} // namespace

OpenWindows Sidebar::OpenContentWindows(WindowStack &stack,
                                        const Gamestate &gamestate,
                                        const World &world) {
    const Options &options = world.Settings();
    OpenWindows open;

    for (const World::OpenContainer &window : world.OpenContainers) {
        const auto container = gamestate.Containers.find(window.Id);

        if (container != gamestate.Containers.end()) {
            stack.Open(std::make_unique<ContainerWindow>(container->second,
                                                         window.InitialItems));
        }
    }

    /* Nobody is there to press the buttons of the other windows, so they
     * are opened once the containers have taken the room they need, sized
     * as if the buttons had been pressed just now. */
    if (options.ShowSkillsWindow) {
        open.Skills = stack.OpenIfRoom(std::make_unique<SkillsWindow>(
                gamestate.Player,
                gamestate.Version.Features.CapacityDivisor));
    }

    if (options.ShowBattleWindow) {
        open.Battle = stack.OpenIfRoom(
                std::make_unique<BattleWindow>(gamestate, world.Battle));
    }

    if (options.ShowVIPWindow) {
        open.VIP = stack.OpenIfRoom(std::make_unique<VIPWindow>(world.Buddies));
    }

    return open;
}

void Sidebar::DrawFixedPart(Canvas &canvas,
                            int x,
                            int y,
                            const Gamestate &gamestate,
                            const World &world,
                            const OpenWindows &open,
                            uint32_t now) {
    const int left = x + Border;
    int top = 0;

    /* Starts a panel of the given height below the previous one, returning
     * where its top is. The background is one texture running down all of
     * them. */
    const auto panel = [&](int height) {
        const int panelTop = y + Border + top;

        Tools.Pieces.Tile(canvas,
                          Skin::Background,
                          Rect{left, panelTop, PanelWidth, height},
                          0,
                          top % Skin::Height(Skin::Background));
        top += height;

        return panelTop;
    };

    DrawFrame(canvas, Tools.Pieces, x, y);

    DrawAutomapPanel(canvas,
                     Tools,
                     left,
                     panel(AutomapPanelHeight),
                     world.Chart,
                     gamestate.Map.Position);
    DrawStatusPanel(canvas,
                    Tools,
                    left,
                    panel(StatusPanelHeight),
                    gamestate.Player);
    DrawInventoryPanel(canvas,
                       Tools,
                       left,
                       panel(InventoryPanelHeight),
                       gamestate,
                       now);
    DrawButtonBar(canvas, Tools, left, panel(ButtonBarHeight), open);
}

void Sidebar::Draw(Canvas &canvas,
                   const Rect &area,
                   const Gamestate &gamestate,
                   const World &world,
                   uint32_t now) {
    const ClipScope clip(canvas, area);
    WindowStack stack(area.Height - FixedHeight);
    const OpenWindows open = OpenContentWindows(stack, gamestate, world);

    DrawFixedPart(canvas, area.X, area.Y, gamestate, world, open, now);
    stack.Draw(canvas, Tools, area.X, area.Y + FixedHeight, now);
}

} // namespace Client
} // namespace trc
