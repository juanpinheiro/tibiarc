#
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


# LZMA decoder from the 7-Zip SDK (public domain), used by `.recording` and
# `.cam` containers.
add_library(tibiarc_lzma STATIC
  "7z/Alloc.c"
  "7z/Lzma2Dec.c"
  "7z/LzmaDec.c"
)
add_library(tibiarc::lzma ALIAS tibiarc_lzma)
target_include_directories(tibiarc_lzma PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# nlohmann/json (MIT), header-only.
add_library(tibiarc_json INTERFACE)
add_library(tibiarc::json ALIAS tibiarc_json)
target_include_directories(tibiarc_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
