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


# Command-line tools built on top of the libraries.

add_library(tibiarc_apps_shared STATIC
  "shared/collation.cpp"
  "shared/memoryfile.cpp"
  "shared/png.cpp"
)
target_include_directories(tibiarc_apps_shared PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tibiarc_apps_shared
  PUBLIC tibiarc::core
  PRIVATE tibiarc::options)

add_subdirectory(collator)
add_subdirectory(miner)
add_subdirectory(render)
