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
# It performs no I/O and has no platform dependencies.

add_library(tibiarc_core STATIC
  "assets/pictures.cpp"
  "assets/types.cpp"
  "assets/versions.cpp"
  "common/characterset.cpp"
  "common/crypto.cpp"
  "game/gamestate.cpp"
  "game/tile.cpp"
  "graphics/canvas.cpp"
  "graphics/sprites.cpp"
  "protocol/events.cpp"
  "protocol/parser.cpp"
  "recording/formats/cam.cpp"
  "recording/formats/rec.cpp"
  "recording/formats/tibiacast.cpp"
  "recording/formats/tibiareplay.cpp"
  "recording/formats/tmv1.cpp"
  "recording/formats/tmv2.cpp"
  "recording/formats/ttm.cpp"
  "recording/formats/yatc.cpp"
  "recording/recordings.cpp"
)
add_library(tibiarc::core ALIAS tibiarc_core)

target_include_directories(tibiarc_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tibiarc_core PRIVATE tibiarc::options tibiarc::lzma)

# zlib is needed by the TibiaMovie and Tibiacast containers. Emscripten ships
# it as a port; elsewhere it is an optional system dependency.
if(EMSCRIPTEN)
  target_compile_options(tibiarc_core PRIVATE --use-port=zlib)
  target_link_options(tibiarc_core PUBLIC --use-port=zlib)
else()
  find_package(ZLIB QUIET)
  if(ZLIB_FOUND)
    target_link_libraries(tibiarc_core PRIVATE ZLIB::ZLIB)
  else()
    message(WARNING "zlib was not found: .tmv, .tmv2 and .recording files "
                    "will not be readable")
    target_compile_definitions(tibiarc_core PRIVATE DISABLE_ZLIB)
  endif()
endif()
