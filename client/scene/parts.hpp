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

#ifndef __TRC_CLIENT_SCENE_PARTS_HPP__
#define __TRC_CLIENT_SCENE_PARTS_HPP__

#include "assets/types.hpp"
#include "graphics/sprites.hpp"

namespace trc {
namespace Client {

/* The frame group every type has, and the only one of types that don't tell
 * standing from walking. */
const EntityType::FrameGroup &DefaultGroup(const EntityType &type);

/* The sprite of one 32 by 32 part of a type; `x` and `y` count leftwards
 * and upwards from the bottom-right part. */
const Sprite *PartSprite(const EntityType::FrameGroup &group,
                         int x,
                         int y,
                         int layer,
                         int patternX,
                         int patternY,
                         int patternZ,
                         int phase);

/* Which of the eight pictures of a stackable type shows a pile of `count`;
 * they are laid out four to a pattern row. */
int StackPicture(int count);

} // namespace Client
} // namespace trc

#endif
