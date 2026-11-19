#
# Copyright 2024 "Simon Sandström"
# Copyright 2024-2026 "John Högberg"
#
# This file is part of tibiarc.
#
# tibiarc is free software: you can redistribute it and/or modify it under the
# terms of the GNU Affero General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.
#
# tibiarc is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License
# along with tibiarc. If not, see <https://www.gnu.org/licenses/>.
#


# `tibiarc::core`: everything needed to turn a recording into game state.
# `tibiarc::client`: turns game state into the picture the original client
# would have shown. It draws into plain canvases and performs no I/O.

add_library(tibiarc_client STATIC
  "playback/session.cpp"
  "resources/fonts.cpp"
  "resources/skin.cpp"
  "scene/animation.cpp"
  "scene/light.cpp"
  "scene/mapview.cpp"
  "scene/outfits.cpp"
  "scene/overlay.cpp"
  "world/console.cpp"
  "world/talk.cpp"
  "world/world.cpp"
  "viewer.cpp"
)
add_library(tibiarc::client ALIAS tibiarc_client)

target_include_directories(tibiarc_client PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tibiarc_client
  PUBLIC tibiarc::core
  PRIVATE tibiarc::options
)
