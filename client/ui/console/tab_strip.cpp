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

#include "ui/console/tab_strip.hpp"

#include <string>

namespace trc {
namespace Client {

/* Skins of a tab, both of the same size. */
enum TabSkin : int {
    TabActive = 186,
    TabInactive = 187,
};

static const Pixel ActiveColor(224, 224, 224);
static const Pixel InactiveColor(128, 128, 128);
static const Pixel UnreadColor(248, 96, 96);
static const Pixel FlashColor(248, 248, 248);

/* The caption is centred around this point of the tab. */
static constexpr int CaptionX = 48;
static constexpr int CaptionY = 4;

/* Room kept free between the caption and either side of the tab. */
static constexpr int CaptionMargin = 4;

/* The client's clock ticks at this pace, and the caption of a tab lights
 * up for this many ticks whenever something new is said in its channel. */
static constexpr uint32_t TickTime = 100;
static constexpr uint32_t FlashTicks = 10;

/* Captions that are too wide are cut short, ending in three dots. */
static std::string Caption(const Font &font, const std::string &name) {
    static constexpr size_t Dots = 3;

    const size_t fit =
            font.TextFit(name, Skin::Width(TabActive) - 2 * CaptionMargin);

    if (fit >= name.size()) {
        return name;
    }

    return name.substr(0, fit < Dots ? 0 : fit - Dots) +
           std::string(Dots, '.');
}

static Pixel CaptionColor(const ConsoleChannel &channel,
                          bool active,
                          uint32_t now) {
    if (active) {
        return ActiveColor;
    } else if (!channel.UnreadSince) {
        return InactiveColor;
    }

    const uint32_t flashEnd = *channel.UnreadSince / TickTime + FlashTicks;

    return (now / TickTime <= flashEnd) ? FlashColor : UnreadColor;
}

bool DrawTabStrip(Canvas &canvas,
                  const Fonts &fonts,
                  const Skin &skin,
                  const Rect &area,
                  const ConsoleLog &log,
                  uint32_t now) {
    const int tabWidth = Skin::Width(TabActive);
    int x = area.X;

    for (const auto &channel : log.All()) {
        /* Nothing in a recording tells which tab the player was looking
         * at, so the default channel is the one in front throughout. */
        const bool active = (channel.get() == &log.Default());

        /* Tabs are never squeezed or cut off. */
        if (x + tabWidth > area.Right()) {
            return true;
        }

        skin.Draw(canvas, active ? TabActive : TabInactive, x, area.Y);
        fonts.Game.Draw(canvas,
                        x + CaptionX,
                        area.Y + CaptionY,
                        CaptionColor(*channel, active, now),
                        Caption(fonts.Game, channel->Name),
                        Align::Center);

        x += tabWidth;
    }

    return false;
}

} // namespace Client
} // namespace trc
