/*
 * Copyright 2011-2016 "Silver Squirrel Software Handelsbolag"
 * Copyright 2023-2024 "John Högberg"
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

#ifndef __TRC_MAP_HPP__
#define __TRC_MAP_HPP__

#include <algorithm>
#include <array>
#include <cstdint>

#include "game/effect.hpp"
#include "common/position.hpp"
#include "game/tile.hpp"

#include "common/utils.hpp"

namespace trc {
struct Map {
    static constexpr int TileBufferWidth = 18;
    static constexpr int TileBufferHeight = 14;
    static constexpr int TileBufferDepth = 8;

    /* Ambient light does not change at once: the client moves its level
     * and each colour channel one step towards the target every second. */
    struct AmbientLight {
        static constexpr uint32_t StepInterval = 1000;

        struct Value {
            int Level = 0;
            int Red = 0, Green = 0, Blue = 0;
        };

        Value From, Target;
        uint32_t ChangeTick = 0;
        bool Known = false;

        void Set(uint8_t level, uint8_t color, uint32_t tick) {
            const Value target{level,
                               (color / 36) * 51,
                               ((color / 6) % 6) * 51,
                               (color % 6) * 51};

            From = Known ? At(tick) : target;
            Target = target;
            ChangeTick = tick;
            Known = true;
        }

        Value At(uint32_t tick) const {
            const int steps = static_cast<int>((tick / StepInterval) -
                                               (ChangeTick / StepInterval));
            auto approach = [steps](int from, int target) {
                return (from < target) ? std::min(target, from + steps)
                                       : std::max(target, from - steps);
            };

            return Value{approach(From.Level, Target.Level),
                         approach(From.Red, Target.Red),
                         approach(From.Green, Target.Green),
                         approach(From.Blue, Target.Blue)};
        }
    } Light;

    trc::Position Position;

    const trc::Tile &Tile(const trc::Position &position) const {
        return Tile(position.X, position.Y, position.Z);
    }

    const trc::Tile &Tile(int X, int Y, int Z) const {
        trc::Assert(X >= 0 && Y >= 0 && Z >= 0);

        X %= TileBufferWidth;
        Y %= TileBufferHeight;
        Z %= TileBufferDepth;

        return Tiles[X + (Y + (Z * TileBufferHeight)) * TileBufferWidth];
    }

    trc::Tile &Tile(const trc::Position &position) {
        return Tile(position.X, position.Y, position.Z);
    }

    trc::Tile &Tile(int X, int Y, int Z) {
        Assert(X >= 0 && Y >= 0 && Z >= 0);

        X %= TileBufferWidth;
        Y %= TileBufferHeight;
        Z %= TileBufferDepth;

        return Tiles[X + (Y + (Z * TileBufferHeight)) * TileBufferWidth];
    }

    void Clear() {
        for (auto &tile : Tiles) {
            tile.Clear();
        }
    }

private:
    std::array<trc::Tile, TileBufferWidth * TileBufferHeight * TileBufferDepth>
            Tiles;
};
} // namespace trc

#endif /* __TRC_MAP_HPP__ */
