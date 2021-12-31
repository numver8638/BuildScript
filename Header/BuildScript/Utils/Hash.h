/*
 * Hash.h
 * - Hash functions.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_HASH_H
#define BUILDSCRIPT_UTILS_HASH_H

#include <array>

#include <BuildScript/Config.h>

namespace BuildScript {
    /**
     * @brief Calculate SHA1 hash.
     */
    class EXPORT_API SHA1 final {
    public:
        using Digest = std::array<uint8_t, 20>;
        static constexpr size_t BlockCount = 16;
        static constexpr size_t BlockSize = sizeof(uint32_t) * BlockCount;

    private:
        std::array<uint32_t, 5> m_hashes = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
        uint32_t m_block[BlockCount] = {};
        size_t m_totalLength = 0;
        size_t m_length = 0;
        bool m_finalized = false;

        uint32_t& Get(size_t i) { return m_block[i % BlockCount]; }

        void Hash();

    public:
        /**
         * @brief Update hash.
         * @param buffer a data for update hash.
         * @param length a length of the data.
         * @warning After call @c GetDigest() member function, cannot call this function.
         */
        void Update(const void* buffer, size_t length);

        /**
         * @brief Get hash and finalize it.
         * @return a @c SHA1::Digest that representing SHA-1 hash.
         * @warning After call this member function, @c Update() cannot be called.
         */
        Digest GetDigest();
    }; // end class SHA1
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_HASH_H