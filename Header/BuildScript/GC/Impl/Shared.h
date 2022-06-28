/*
 * Shared.h
 * - Shared codes among GC implementations.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_IMPL_SHARED_H
#define BUILDSCRIPT_GC_IMPL_SHARED_H

#include <memory>

#include <BuildScript/GC/ObjectHeader.h>
#include <BuildScript/Platform/MemoryChunk.h>
#include <BuildScript/Utils/PointerArith.h>

inline void* AllocateFromChunk(BuildScript::MemoryChunk& chunk, size_t size, void* &top, size_t &used) {
    if (chunk.GetCommittedSize() <= (size + used)) {
        const auto PAGE_SIZE = BuildScript::MemoryChunk::GetPageSize();

        auto isFull = (chunk.GetCommittedSize() == chunk.GetReservedSize());
        auto requestSize = PAGE_SIZE * ((size / PAGE_SIZE) + 1);
        auto isOverflow = (chunk.GetCommittedSize() + requestSize > chunk.GetReservedSize());

        if (isFull || isOverflow) { return nullptr; }

        chunk.Commit(requestSize);
    }

    auto* ptr = top;

    top = add(top, size);
    used += size;

    std::memset(ptr, 0, size);

    return ptr;
}

inline size_t CalibrateSize(size_t size) {
    using namespace BuildScript;

    size += (OBJECT_ALIGNMENT - (size % OBJECT_ALIGNMENT));
    size += MAX_HEADER_SIZE;

    return size;
}

#endif // BUILDSCRIPT_GC_IMPL_SHARED_H