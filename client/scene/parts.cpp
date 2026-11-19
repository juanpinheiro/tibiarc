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

#include "scene/parts.hpp"

#include <utility>

namespace trc {
namespace Client {

const EntityType::FrameGroup &DefaultGroup(const EntityType &type) {
    return type.FrameGroups[std::to_underlying(FrameGroupIndex::Default)];
}

const Sprite *PartSprite(const EntityType::FrameGroup &group,
                         int x,
                         int y,
                         int layer,
                         int patternX,
                         int patternY,
                         int patternZ,
                         int phase) {
    size_t index = phase % group.FrameCount;

    index = index * group.ZDiv + (patternZ % group.ZDiv);
    index = index * group.YDiv + (patternY % group.YDiv);
    index = index * group.XDiv + (patternX % group.XDiv);
    index = index * group.LayerCount + layer;
    index = index * (group.SizeX * group.SizeY) + y * group.SizeX + x;

    if (index >= group.Sprites.size()) {
        return nullptr;
    }

    const Sprite *sprite = group.Sprites[index];

    return (sprite != nullptr && sprite->Buffer != nullptr) ? sprite : nullptr;
}

int StackPicture(int count) {
    static constexpr int Thresholds[] = {2, 3, 4, 5, 10, 25, 50};
    int picture = 0;

    for (int threshold : Thresholds) {
        picture += (count >= threshold);
    }

    return picture;
}

} // namespace Client
} // namespace trc
