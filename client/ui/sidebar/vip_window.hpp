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

#ifndef __TRC_CLIENT_UI_SIDEBAR_VIP_WINDOW_HPP__
#define __TRC_CLIENT_UI_SIDEBAR_VIP_WINDOW_HPP__

#include "ui/sidebar/content_window.hpp"
#include "world/world.hpp"

#include <map>
#include <vector>

namespace trc {
namespace Client {

/* The "VIP" window: the names of the player's buddies, those who are
 * online first and in green, the others in red. Within each group they
 * are in alphabetical order. */
class VIPWindow : public ContentWindow {
public:
    explicit VIPWindow(const std::map<uint32_t, World::Buddy> &buddies);

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
    /* In the order they are listed. */
    std::vector<const World::Buddy *> Buddies;
};

} // namespace Client
} // namespace trc

#endif
