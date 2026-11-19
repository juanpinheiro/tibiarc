#
# Copyright 2024 "Simon Sandström"
# Copyright 2024-2026 "John Högberg"
# Copyright 2026 "Juan Pinheiro"
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


cmake_minimum_required(VERSION 3.25)

project(tibiarc
  VERSION 0.2
  LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TIBIARC_SANITIZE "Enable address and undefined-behaviour sanitizers" OFF)
option(TIBIARC_COVERAGE "Enable code coverage instrumentation" OFF)

include(cmake/TibiarcOptions.cmake)

add_subdirectory(third_party)
add_subdirectory(core)
add_subdirectory(client)

if(EMSCRIPTEN)
  add_subdirectory(platform/wasm)
else()
  add_subdirectory(apps)

  include(CTest)
  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()
