/*
 * MemoryChunk.h
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_PLATFORM_MEMORYCHUNK_H
#define BUILDSCRIPT_PLATFORM_MEMORYCHUNK_H

#include <cstddef>

#include "BuildScript/Utils/NonCopyable.h"

namespace BuildScript {
    /**
     * @brief .
     */
    class MemoryChunk final : NonCopyable {
    private:
        const size_t m_size;

        void* m_base = nullptr;
        size_t m_commitSize = 0;

    public:
        explicit MemoryChunk(size_t size);

        ~MemoryChunk() noexcept;

        void Commit(size_t size);

        void Release(size_t size);

        void* GetBase() const { return m_base; }

        size_t GetReservedSize() const { return m_size; }

        size_t GetCommittedSize() const { return m_commitSize; }

        static size_t GetPageSize();
    }; // end class MemoryChunk
} // end namespace BuildScript

#endif // BUILDSCRIPT_PLATFORM_MEMORYCHUNK_H