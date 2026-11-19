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

#ifndef __TRC_CLIENT_UI_SIDEBAR_SKILLS_WINDOW_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_SKILLS_WINDOW_HPP__

#include "game/player.hpp"
#include "ui/sidebar/content_window.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace trc {
namespace Client {

/* The "Skills" window: the player's figures one to a line, with a bar
 * under those that the player advances in showing how far along the next
 * step is. A rule separates the fighting skills from the rest. */
class SkillsWindow : public ContentWindow {
public:
    /* `capacityDivisor` turns the capacity the server reports into the
     * unit the window shows. */
    SkillsWindow(const PlayerData &player, int capacityDivisor);

    int InitialHeight() const override;

protected:
    std::string_view Title() const override;
    void DrawIcon(Canvas &canvas,
                  Widgets &widgets,
                  int x,
                  int y,
                  uint32_t now) const override;
    int ContentHeight() const override;
    int PageHeight(int bodyHeight) const override;
    void DrawList(Canvas &canvas,
                  Widgets &widgets,
                  const Rect &area,
                  uint32_t now) const override;

private:
    struct Line {
        enum class Kind { Figure, Advancing, Skill, Rule };

        Kind What;
        std::string Caption;
        int64_t Value = 0;
        /* How far along the next step is, for the kinds with a bar. */
        int Percent = 0;

        bool HasBar() const {
            return What == Kind::Advancing || What == Kind::Skill;
        }
    };

    std::vector<Line> Lines;
};

} // namespace Client
} // namespace trc

#endif
