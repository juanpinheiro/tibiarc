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

#ifndef __TRC_CLIENT_SCENE_OUTFITS_HPP__
#define __TRC_CLIENT_SCENE_OUTFITS_HPP__

#include "common/pixel.hpp"
#include "game/creature.hpp"
#include "graphics/sprites.hpp"

#include <cstdint>
#include <unordered_map>

namespace trc {
namespace Client {

/* Produces coloured outfit sprites. An outfit sprite comes with a mask
 * whose pure yellow, red, green and blue pixels mark the head, body, legs
 * and feet; those areas are replaced by the chosen colour modulated with
 * the brightness of the base sprite. Results are cached. */
class OutfitColorizer {
public:
    static Pixel PaletteColor(int index);

    const Sprite &Get(const Sprite &base,
                      const Sprite &mask,
                      const Appearance &colors);

private:
    struct Key {
        const Sprite *Base;
        const Sprite *Mask;
        uint32_t Colors;

        bool operator==(const Key &) const = default;
    };

    struct KeyHash {
        size_t operator()(const Key &key) const {
            return std::hash<const void *>()(key.Base) * 31 ^
                   std::hash<const void *>()(key.Mask) * 17 ^ key.Colors;
        }
    };

    static constexpr size_t MaxEntries = 4096;

    std::unordered_map<Key, Sprite, KeyHash> Cache;
};

} // namespace Client
} // namespace trc

#endif
