/*
 * Copyright 2026 "tibiarc contributors"
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

#include "shared/png.hpp"

#include "common/utils.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <vector>

namespace trc {
namespace PNG {

namespace {

uint32_t Crc32(const uint8_t *data, size_t size, uint32_t crc = 0) {
    static const auto table = [] {
        std::array<uint32_t, 256> result;

        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;

            for (int bit = 0; bit < 8; bit++) {
                c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            }

            result[i] = c;
        }

        return result;
    }();

    crc = ~crc;

    while (size--) {
        crc = table[(crc ^ *data++) & 0xFF] ^ (crc >> 8);
    }

    return ~crc;
}

void PutU32(std::vector<uint8_t> &out, uint32_t value) {
    out.push_back(value >> 24);
    out.push_back(value >> 16);
    out.push_back(value >> 8);
    out.push_back(value);
}

void PutChunk(std::ofstream &file,
              const char *tag,
              const std::vector<uint8_t> &payload) {
    std::vector<uint8_t> chunk;

    PutU32(chunk, static_cast<uint32_t>(payload.size()));
    chunk.insert(chunk.end(), tag, tag + 4);
    chunk.insert(chunk.end(), payload.begin(), payload.end());
    PutU32(chunk, Crc32(chunk.data() + 4, chunk.size() - 4));

    file.write(reinterpret_cast<const char *>(chunk.data()), chunk.size());
}

} // namespace

void Write(const Canvas &canvas, const std::filesystem::path &path) {
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        throw IOError();
    }

    static const uint8_t signature[] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    file.write(reinterpret_cast<const char *>(signature), sizeof(signature));

    std::vector<uint8_t> header;
    PutU32(header, canvas.Width);
    PutU32(header, canvas.Height);
    header.insert(header.end(), {8, 2, 0, 0, 0});
    PutChunk(file, "IHDR", header);

    /* One filter byte per row followed by RGB triplets. */
    std::vector<uint8_t> raw;
    raw.reserve((canvas.Width * 3 + 1) * canvas.Height);

    for (int y = 0; y < canvas.Height; y++) {
        raw.push_back(0);

        for (int x = 0; x < canvas.Width; x++) {
            const Pixel &pixel = canvas.GetPixel(x, y);

            raw.push_back(pixel.Red);
            raw.push_back(pixel.Green);
            raw.push_back(pixel.Blue);
        }
    }

    /* A zlib stream of stored blocks: no compression, no dependencies. */
    std::vector<uint8_t> stream = {0x78, 0x01};
    uint32_t a = 1, b = 0;

    for (size_t offset = 0; offset < raw.size() || offset == 0;) {
        const size_t length = std::min<size_t>(0xFFFF, raw.size() - offset);
        const bool last = offset + length >= raw.size();

        stream.push_back(last ? 1 : 0);
        stream.push_back(length & 0xFF);
        stream.push_back(length >> 8);
        stream.push_back(~length & 0xFF);
        stream.push_back((~length >> 8) & 0xFF);
        stream.insert(stream.end(),
                      raw.begin() + offset,
                      raw.begin() + offset + length);

        for (size_t i = 0; i < length; i++) {
            a = (a + raw[offset + i]) % 65521;
            b = (b + a) % 65521;
        }

        offset += length;

        if (last) {
            break;
        }
    }

    PutU32(stream, (b << 16) | a);
    PutChunk(file, "IDAT", stream);
    PutChunk(file, "IEND", {});
}

} // namespace PNG
} // namespace trc
