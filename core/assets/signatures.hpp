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

#ifndef __TRC_SIGNATURES_HPP__
#define __TRC_SIGNATURES_HPP__

#include "assets/versions_decl.hpp"
#include "common/datareader.hpp"

namespace trc {

/* Tells which game version a `Tibia.dat` belongs to from the signature it
 * starts with. Releases that shipped the same file are told apart by
 * nothing, so the earliest of them is reported.
 *
 * Returns false for files that aren't known. */
bool IdentifyTypeFile(DataReader data, VersionTriplet &triplet);

} // namespace trc

#endif /* __TRC_SIGNATURES_HPP__ */
