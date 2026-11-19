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

#include "assets/signatures.hpp"

#include "assets/versions.hpp"

#include <cstdint>

namespace trc {

namespace {

struct KnownFile {
    uint32_t Signature;
    VersionTriplet Version;
};

/* Sorted by version; a signature is listed once, with the first release
 * that carried it. */
const KnownFile KnownTypeFiles[] = {
        {0x3DFF4B2A, {7, 10, 0}},
        {0x3FD4FB91, {7, 13, 0}},
        {0x3FDF40C6, {7, 21, 0}},
        {0x404E1C14, {7, 23, 0}},
        {0x411A6233, {7, 30, 0}},
        {0x41BF619C, {7, 40, 0}},
        {0x42F81973, {7, 50, 0}},
        {0x439D5A33, {7, 60, 0}},
        {0x44CE4743, {7, 80, 0}},
        {0x457D854E, {7, 90, 0}},
        {0x459E7B73, {7, 92, 0}},
        {0x467FD7E6, {8, 0, 0}},
        {0x47F60E37, {8, 11, 0}},
        {0x486905AA, {8, 20, 0}},
        {0x486CCA2B, {8, 21, 0}},
        {0x489980A1, {8, 22, 0}},
        {0x48DA1FB6, {8, 30, 0}},
        {0x493D607A, {8, 40, 0}},
        {0x49B7CC19, {8, 41, 0}},
        {0x49C233C9, {8, 42, 0}},
        {0x4A49C5EB, {8, 50, 0}},
        {0x4A4CC0DC, {8, 52, 0}},
        {0x4AE97492, {8, 53, 0}},
        {0x4B1E2CAA, {8, 54, 0}},
        {0x4B98FF53, {8, 55, 0}},
        {0x4C28B721, {8, 60, 0}},
        {0x4C6A4CBC, {8, 61, 0}},
        {0x4C973450, {8, 62, 0}},
        {0x4CFE22C5, {8, 70, 0}},
        {0x4D41979E, {8, 71, 0}},
};

} // namespace

bool IdentifyTypeFile(DataReader data, VersionTriplet &triplet) {
    if (data.Remaining() < sizeof(uint32_t)) {
        return false;
    }

    const uint32_t signature = data.ReadU32();

    for (const auto &known : KnownTypeFiles) {
        if (known.Signature == signature) {
            triplet = known.Version;
            return true;
        }
    }

    return false;
}

} // namespace trc
