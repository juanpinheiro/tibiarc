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


# Unit tests of the pure parts of the client run everywhere. The sample
# recordings need the game's data files, which cannot be distributed: point
# TIBIARC_TEST_DATA at a folder holding `<version>/Tibia.{dat,spr,pic}` to
# enable them.

set(TIBIARC_TEST_DATA "${CMAKE_CURRENT_SOURCE_DIR}/data" CACHE PATH
    "Folder with one sub-folder of game data files per version")

add_executable(unit_tests
  "unit/animation_test.cpp"
  "unit/hotspots_test.cpp"
  "unit/layout_test.cpp"
  "unit/main.cpp"
)
target_link_libraries(unit_tests PRIVATE tibiarc::options tibiarc::client)
add_test(NAME "unit" COMMAND unit_tests)

function(add_sample_tests version recording)
  set(data "${TIBIARC_TEST_DATA}/${version}")
  set(sample "${CMAKE_CURRENT_SOURCE_DIR}/samples/${version}/${recording}")

  if(NOT EXISTS "${data}/Tibia.dat" OR
     NOT EXISTS "${data}/Tibia.spr" OR
     NOT EXISTS "${data}/Tibia.pic")
    message(STATUS "No ${version} data files in ${TIBIARC_TEST_DATA}: "
                   "skipping tests of ${recording}")
    return()
  endif()

  # Parses the whole recording.
  add_test(NAME "parse: ${version}/${recording}"
           COMMAND miner --dry-run --input-version ${version}
                   "${data}" "${sample}")

  # Paints a frame every few seconds, at two window sizes; this exercises
  # nearly all of the client without judging the picture.
  foreach(size "550;450" "1280;720")
    list(GET size 0 width)
    list(GET size 1 height)
    add_test(NAME "render ${width}x${height}: ${version}/${recording}"
             COMMAND render --input-version ${version}
                     --size ${width} ${height}
                     --step 4000 --count 50 --discard
                     "${data}" "${sample}" "unused")
  endforeach()
endfunction()

add_sample_tests(8.40 sample.tmv2)
add_sample_tests(8.50 sample.yatc)
