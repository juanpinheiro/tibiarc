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


add_executable(collator "collator.cpp")
target_link_libraries(collator PRIVATE
  tibiarc::options tibiarc::core tibiarc_apps_shared)

# `std::execution::par` needs TBB with libstdc++; without it the collator
# still works, only serially.
find_package(TBB QUIET)
if(TBB_FOUND)
  target_link_libraries(collator PRIVATE TBB::tbb)
endif()
