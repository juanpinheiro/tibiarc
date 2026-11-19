/*
 * Copyright 2025-2026 "John Högberg"
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

/* Self-contained AES-256 (ECB, decryption only, PKCS#7 padding) and SHA-1.
 *
 * These exist solely to decode recording containers, never to protect
 * anything, so a small portable implementation is preferable to pulling in a
 * platform crypto library: it behaves identically natively and under
 * WebAssembly. */

#include "common/crypto.hpp"

#include <array>
#include <bit>
#include <cstring>

namespace trc {
namespace Crypto {
namespace {

constexpr uint8_t GFMul2(uint8_t x) {
    return static_cast<uint8_t>((x << 1) ^ ((x & 0x80) ? 0x1B : 0));
}

constexpr uint8_t GFMul(uint8_t a, uint8_t b) {
    uint8_t result = 0;

    for (int bit = 0; bit < 8; bit++) {
        if (b & 1) {
            result ^= a;
        }

        a = GFMul2(a);
        b >>= 1;
    }

    return result;
}

struct Tables {
    std::array<uint8_t, 256> Forward{};
    std::array<uint8_t, 256> Inverse{};

    constexpr Tables() {
        /* Generate the S-box from the multiplicative inverse followed by the
         * affine transform, walking the field with generator 3. */
        uint8_t p = 1, q = 1;

        do {
            p = static_cast<uint8_t>(p ^ GFMul2(p));

            q ^= q << 1;
            q ^= q << 2;
            q ^= q << 4;
            q ^= (q & 0x80) ? 0x09 : 0;

            uint8_t x = static_cast<uint8_t>(q ^ std::rotl(q, 1) ^
                                             std::rotl(q, 2) ^
                                             std::rotl(q, 3) ^
                                             std::rotl(q, 4));
            x ^= 0x63;

            Forward[p] = x;
            Inverse[x] = p;
        } while (p != 1);

        Forward[0] = 0x63;
        Inverse[0x63] = 0;
    }
};

constexpr Tables SBox;

class PortableAES : public AES_ECB_256 {
    static constexpr int Rounds = 14;
    static constexpr int BlockSize = 16;

    uint8_t RoundKeys[(Rounds + 1) * BlockSize];

    void DecryptBlock(const uint8_t *input, uint8_t *output) const {
        uint8_t state[BlockSize];

        for (int i = 0; i < BlockSize; i++) {
            state[i] = input[i] ^ RoundKeys[Rounds * BlockSize + i];
        }

        for (int round = Rounds - 1; round >= 0; round--) {
            uint8_t shifted[BlockSize];

            /* Inverse ShiftRows and SubBytes. */
            for (int column = 0; column < 4; column++) {
                for (int row = 0; row < 4; row++) {
                    shifted[((column + row) % 4) * 4 + row] =
                            SBox.Inverse[state[column * 4 + row]];
                }
            }

            for (int i = 0; i < BlockSize; i++) {
                shifted[i] ^= RoundKeys[round * BlockSize + i];
            }

            if (round == 0) {
                memcpy(state, shifted, BlockSize);
                break;
            }

            /* Inverse MixColumns. */
            for (int column = 0; column < 4; column++) {
                const uint8_t *in = &shifted[column * 4];
                uint8_t *out = &state[column * 4];

                out[0] = GFMul(in[0], 14) ^ GFMul(in[1], 11) ^
                         GFMul(in[2], 13) ^ GFMul(in[3], 9);
                out[1] = GFMul(in[0], 9) ^ GFMul(in[1], 14) ^
                         GFMul(in[2], 11) ^ GFMul(in[3], 13);
                out[2] = GFMul(in[0], 13) ^ GFMul(in[1], 9) ^
                         GFMul(in[2], 14) ^ GFMul(in[3], 11);
                out[3] = GFMul(in[0], 11) ^ GFMul(in[1], 13) ^
                         GFMul(in[2], 9) ^ GFMul(in[3], 14);
            }
        }

        memcpy(output, state, BlockSize);
    }

public:
    PortableAES(const uint8_t key[32]) {
        memcpy(RoundKeys, key, 32);

        uint8_t rcon = 1;

        for (int i = 8; i < (Rounds + 1) * 4; i++) {
            uint8_t word[4];

            memcpy(word, &RoundKeys[(i - 1) * 4], 4);

            if (i % 8 == 0) {
                const uint8_t first = word[0];

                word[0] = SBox.Forward[word[1]] ^ rcon;
                word[1] = SBox.Forward[word[2]];
                word[2] = SBox.Forward[word[3]];
                word[3] = SBox.Forward[first];

                rcon = GFMul2(rcon);
            } else if (i % 8 == 4) {
                for (auto &byte : word) {
                    byte = SBox.Forward[byte];
                }
            }

            for (int j = 0; j < 4; j++) {
                RoundKeys[i * 4 + j] = RoundKeys[(i - 8) * 4 + j] ^ word[j];
            }
        }
    }

    size_t Decrypt(const uint8_t *input,
                   size_t inputLength,
                   uint8_t *output,
                   size_t maxLength) override {
        if (inputLength == 0 || (inputLength % BlockSize) != 0 ||
            maxLength < inputLength) {
            throw InvalidDataError();
        }

        for (size_t offset = 0; offset < inputLength; offset += BlockSize) {
            DecryptBlock(&input[offset], &output[offset]);
        }

        const uint8_t padding = output[inputLength - 1];

        if (padding == 0 || padding > BlockSize) {
            throw InvalidDataError();
        }

        for (size_t i = inputLength - padding; i < inputLength; i++) {
            if (output[i] != padding) {
                throw InvalidDataError();
            }
        }

        return inputLength - padding;
    }
};

class PortableSHA1 : public SHA1 {
    static void Compress(uint32_t state[5], const uint8_t block[64]) {
        uint32_t w[80];

        for (int i = 0; i < 16; i++) {
            w[i] = (uint32_t(block[i * 4]) << 24) |
                   (uint32_t(block[i * 4 + 1]) << 16) |
                   (uint32_t(block[i * 4 + 2]) << 8) |
                   uint32_t(block[i * 4 + 3]);
        }

        for (int i = 16; i < 80; i++) {
            w[i] = std::rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = state[0], b = state[1], c = state[2], d = state[3],
                 e = state[4];

        for (int i = 0; i < 80; i++) {
            uint32_t f, k;

            if (i < 20) {
                f = (b & c) | (~b & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            const uint32_t next = std::rotl(a, 5) + f + e + k + w[i];

            e = d;
            d = c;
            c = std::rotl(b, 30);
            b = a;
            a = next;
        }

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;
    }

public:
    void Hash(const uint8_t *input,
              size_t length,
              uint8_t output[Size]) override {
        uint32_t state[5] = {0x67452301,
                             0xEFCDAB89,
                             0x98BADCFE,
                             0x10325476,
                             0xC3D2E1F0};
        size_t offset = 0;

        for (; offset + 64 <= length; offset += 64) {
            Compress(state, &input[offset]);
        }

        uint8_t tail[128] = {};
        const size_t remaining = length - offset;
        const size_t tailLength = (remaining < 56) ? 64 : 128;
        const uint64_t bits = uint64_t(length) * 8;

        if (remaining > 0) {
            memcpy(tail, &input[offset], remaining);
        }

        tail[remaining] = 0x80;

        for (int i = 0; i < 8; i++) {
            tail[tailLength - 1 - i] = static_cast<uint8_t>(bits >> (i * 8));
        }

        for (size_t i = 0; i < tailLength; i += 64) {
            Compress(state, &tail[i]);
        }

        for (int i = 0; i < 5; i++) {
            output[i * 4 + 0] = static_cast<uint8_t>(state[i] >> 24);
            output[i * 4 + 1] = static_cast<uint8_t>(state[i] >> 16);
            output[i * 4 + 2] = static_cast<uint8_t>(state[i] >> 8);
            output[i * 4 + 3] = static_cast<uint8_t>(state[i]);
        }
    }
};

} // namespace

std::unique_ptr<AES_ECB_256> AES_ECB_256::Create(const uint8_t key[32]) {
    return std::make_unique<PortableAES>(key);
}

std::unique_ptr<SHA1> SHA1::Create() {
    return std::make_unique<PortableSHA1>();
}

} // namespace Crypto
} // namespace trc
