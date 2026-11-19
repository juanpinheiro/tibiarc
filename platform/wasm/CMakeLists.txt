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


# The player as a WebAssembly module for the web front end in `web/`.

add_executable(tibiarc_wasm
  "api.cpp"
)
set_target_properties(tibiarc_wasm PROPERTIES
  OUTPUT_NAME "tibiarc"
  SUFFIX ".mjs"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/web/wasm"
)
target_link_libraries(tibiarc_wasm PRIVATE tibiarc::options tibiarc::client)
target_link_options(tibiarc_wasm PRIVATE
  --no-entry
  -sMODULARIZE=1
  -sEXPORT_ES6=1
  -sENVIRONMENT=web,node
  -sALLOW_MEMORY_GROWTH=1
  -sINITIAL_MEMORY=268435456
  -sMAXIMUM_MEMORY=2147483648
  -sFILESYSTEM=0
  "-sEXPORTED_RUNTIME_METHODS=['HEAPU8','UTF8ToString','stringToNewUTF8']"
  "-sEXPORTED_FUNCTIONS=['_malloc','_free']"
)
