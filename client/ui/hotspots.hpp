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

#ifndef __TRC_CLIENT_UI_HOTSPOTS_HPP__
#define __TRC_CLIENT_UI_HOTSPOTS_HPP__

#include "graphics/canvas.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace trc {
namespace Client {

/* What pressing a part of the interface asks for. */
enum class Gesture {
    ToggleSkills,
    ToggleBattle,
    ToggleVIP,
    /* Closes the window of a container; the target tells which. */
    DismissContainer,
    /* Brings a channel of the console to the front; the target is its
     * place among the tabs. */
    SelectChannel,
};

struct Hotspot {
    Rect Area;
    Gesture What;
    uint32_t Target;
};

/* The parts of the window that react to being pressed. They are noted down
 * while the window is painted, so they always match what is on screen. */
class Hotspots {
public:
    void Clear() {
        Spots.clear();
    }

    void Add(const Rect &area, Gesture what, uint32_t target = 0) {
        Spots.push_back(Hotspot{area, what, target});
    }

    /* What is at a point; of overlapping spots the one painted last. */
    std::optional<Hotspot> At(int x, int y) const {
        for (auto it = Spots.rbegin(); it != Spots.rend(); it = std::next(it)) {
            if (it->Area.Contains(x, y)) {
                return *it;
            }
        }

        return std::nullopt;
    }

private:
    std::vector<Hotspot> Spots;
};

} // namespace Client
} // namespace trc

#endif
