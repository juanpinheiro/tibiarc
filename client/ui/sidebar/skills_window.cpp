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

#include "ui/sidebar/skills_window.hpp"

namespace trc {
namespace Client {

namespace {

constexpr int IconSkin = 142;

/* Room left free around the lines, and the distance from one line to the
 * next in the font they are written in. */
constexpr int MarginX = 10, MarginY = 10;
constexpr int LinePitch = 14;

/* A bar lies this far below the top of its line, and makes the line this
 * much taller. The scrollbar counts a pixel less for it. */
constexpr int BarOffset = 12;
constexpr int BarHeight = 5;
constexpr int BarRoom = 10;
constexpr int BarScrollRoom = 9;

const TextStyle CaptionStyle{4, Pixel(176, 176, 176), Align::Left};
const TextStyle ValueStyle{4, Pixel(176, 176, 176), Align::Right};
const Pixel RuleColor(176, 176, 176);
const Pixel AdvancingColor(192, 0, 0);
const Pixel SkillColor(0, 192, 0);

constexpr const char *SkillNames[PLAYER_SKILL_COUNT] = {"Fist Fighting",
                                                        "Club Fighting",
                                                        "Sword Fighting",
                                                        "Axe Fighting",
                                                        "Distance Fighting",
                                                        "Shielding",
                                                        "Fishing"};

/* Writes a number with a comma between every three digits. */
std::string GroupDigits(int64_t number) {
    std::string digits = std::to_string(number);
    const int first = (number < 0) ? 1 : 0;

    for (int at = static_cast<int>(digits.size()) - 3; at > first; at -= 3) {
        digits.insert(static_cast<size_t>(at), 1, ',');
    }

    return digits;
}

} // namespace

SkillsWindow::SkillsWindow(const PlayerData &player, int capacityDivisor) {
    using Kind = Line::Kind;
    const auto &stats = player.Stats;

    Lines = {{Kind::Figure,
              "Experience",
              static_cast<int64_t>(stats.Experience)},
             {Kind::Advancing, "Level", stats.Level, stats.LevelPercent},
             {Kind::Figure, "Hit Points", stats.Health},
             {Kind::Figure, "Mana", stats.Mana},
             {Kind::Figure, "Soul Points", stats.SoulPoints},
             {Kind::Figure, "Capacity", stats.Capacity / capacityDivisor},
             {Kind::Advancing,
              "Magic Level",
              stats.MagicLevel,
              stats.MagicLevelPercent},
             {Kind::Rule, ""}};

    for (int i = 0; i < PLAYER_SKILL_COUNT; i++) {
        Lines.push_back({Kind::Skill,
                         SkillNames[i],
                         player.Skills[i].Effective,
                         player.Skills[i].Percent});
    }
}

int SkillsWindow::InitialHeight() const {
    /* Tall enough to show everything, with the margins. */
    return AtLeastMinimum(ContentHeight() + MarginY * 2);
}

std::string_view SkillsWindow::Title() const {
    return "Skills";
}

void SkillsWindow::DrawIcon(Canvas &canvas,
                            Widgets &widgets,
                            int x,
                            int y,
                            [[maybe_unused]] uint32_t now) const {
    widgets.Pieces.Draw(canvas, IconSkin, x, y);
}

int SkillsWindow::ContentHeight() const {
    int height = 0;

    for (const Line &line : Lines) {
        height += LinePitch + (line.HasBar() ? BarScrollRoom : 0);
    }

    return height;
}

int SkillsWindow::PageHeight(int bodyHeight) const {
    return bodyHeight - MarginY * 2;
}

void SkillsWindow::DrawList(Canvas &canvas,
                            Widgets &widgets,
                            const Rect &area,
                            [[maybe_unused]] uint32_t now) const {
    const Font &font = widgets.Text[CaptionStyle.Font];
    const int left = area.X + MarginX;
    const int width = area.Width - MarginX * 2;
    int top = area.Y + MarginY;

    for (const Line &line : Lines) {
        const int room = LinePitch + (line.HasBar() ? BarRoom : 0);

        if (top >= area.Bottom()) {
            break;
        }

        if (line.What == Line::Kind::Rule) {
            canvas.Fill(Rect{left, top + font.LineHeight() / 2 - 1, width, 1},
                        RuleColor);
        } else {
            const ClipScope clip(canvas, Rect{left, top, width, room});

            widgets.Write(canvas, left, top, CaptionStyle, line.Caption);
            widgets.Write(canvas,
                          left + width - 1,
                          top,
                          ValueStyle,
                          GroupDigits(line.Value));

            if (line.HasBar()) {
                widgets.Gauge(canvas,
                              Rect{left, top + BarOffset, width, BarHeight},
                              line.Percent,
                              line.What == Line::Kind::Skill ? SkillColor
                                                             : AdvancingColor);
            }
        }

        top += room;
    }
}

} // namespace Client
} // namespace trc
