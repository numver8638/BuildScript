/*
 * Hash.cpp
 * - Hash functions.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Hash.h>

#include <algorithm>
#include <cstring>

#include <BuildScript/Assert.h>
#include <BuildScript/Utils/PointerArith.h>

using namespace BuildScript;

inline uint32_t RotateLeft(uint32_t val, int shift) {
    return (val << shift) | (val >> (32 - shift));
}

template <typename T>
T Swap(T val) {
    union {
        T S;
        uint8_t Arr[sizeof(T)];
    };

    S = val;
    T ret = T();

    for (auto i = 0; i < sizeof(T); i++) {
        ret = (ret << 8) | Arr[i];
    }

    return ret;
}

// Reference: https://en.wikipedia.org/wiki/SHA-1
void SHA1::Hash() {
    auto [ A, B, C, D, E ] = m_hashes;

    for (auto i = 0; i < 80; i++) {
        uint32_t F, K;

        if (i >= 16) {
            Get(i) = RotateLeft(Get(i + 13) ^ Get(i + 8) ^ Get(i + 2) ^ Get(i), 1);
        }
        else {
            Get(i) = Swap(Get(i));
        }

        switch (i / 20) {
            case 0:
                F = (B & C) | (~B & D);
                K = 0x5A827999;
                break;

            case 1:
                F = B ^ C ^ D;
                K = 0x6ED9EBA1;
                break;

            case 2:
                F = (B & C) | (B & D) | (C & D);
                K = 0x8F1BBCDC;
                break;

            case 3:
                F = B ^ C ^ D;
                K = 0xCA62C1D6;
                break;

            default:
                NOT_REACHABLE;
        }

        uint32_t temp = RotateLeft(A, 5) + F + E + K + Get(i);
        E = D;
        D = C;
        C = RotateLeft(B, 30);
        B = A;
        A = temp;
    }

    m_hashes[0] += A;
    m_hashes[1] += B;
    m_hashes[2] += C;
    m_hashes[3] += D;
    m_hashes[4] += E;
}

void SHA1::Update(const void* buffer, size_t length) {
    assert(!m_finalized && "Cannot update after call 'GetDigest'.");

    const auto* cursor = reinterpret_cast<const uint8_t*>(buffer);
    m_totalLength += length;

    while (length > 0) {
        auto size = std::min(BlockSize - m_length, length);

        std::memcpy(add(m_block, m_length), cursor, size);

        m_length += size;
        length -= size;
        cursor += size;

        if (m_length == BlockSize) {
            Hash();
            m_length = 0;
        }
    }
}

SHA1::Digest SHA1::GetDigest() {
    static const uint8_t PADDING[64] = { 0x80, 0 };

    auto totalBits = m_totalLength * 8;
    size_t padLength = (m_length < 56) ? (56 - m_length) : ((64 + 56) - m_length);

    Update(PADDING, padLength);

    auto swapped = Swap<uint64_t>(totalBits);
    std::memcpy(m_block + 14, &swapped, sizeof(swapped));

    Hash();

    std::array<uint32_t, 5> hashes
        = { Swap(m_hashes[0]), Swap(m_hashes[1]), Swap(m_hashes[2]), Swap(m_hashes[3]), Swap(m_hashes[4]) };

    Digest digest;
    std::memcpy(digest.data(), hashes.data(), digest.size());
    m_finalized = true;

    return digest;
}