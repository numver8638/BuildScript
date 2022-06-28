/*
 * ObjectHeader.h
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_OBJECTHEADER_H
#define BUILDSCRIPT_GC_OBJECTHEADER_H

#include <cstddef>
#include <cstdint>

namespace BuildScript {
    class GCObject; // Defined in <BuildScript/GC/GCObject.h>

    constexpr size_t OBJECT_ALIGNMENT = 16;
    constexpr size_t MAX_HEADER_SIZE = OBJECT_ALIGNMENT;

    constexpr uint32_t MARK_WHITE = 0;
    constexpr uint32_t MARK_GRAY = 1;
    constexpr uint32_t MARK_BLACK = 2;

    constexpr uint32_t GENERATION_NURSERY = 0;
    constexpr uint32_t GENERATION_SURVIVOR = 1;
    constexpr uint32_t GENERATION_OLD = 2;

    /**
     * @brief .
     */
    struct alignas(OBJECT_ALIGNMENT) GCObjectHeader {
        size_t ObjectSize;          //!< Size of the object.

        uint32_t Mark: 2;           //!< Tricolor mark for collecting unused memory.
        uint32_t Generation: 2;     //!< Generation of the object.

        explicit GCObjectHeader(size_t size)
            : ObjectSize(size), Mark(MARK_WHITE), Generation(GENERATION_NURSERY) {}

        static void* operator new(size_t) = delete;
        static void* operator new[](size_t) = delete;
        static void* operator new(size_t, void* ptr) { return ptr; }

        static void operator delete(void*) = delete;
        static void operator delete[](void*) = delete;
        static void operator delete(void*, void*) { /* do nothing */ }
    }; // end class GCObjectHeader

    static_assert(sizeof(GCObjectHeader) <= MAX_HEADER_SIZE, "Header size exceeds MAX_HEADER_SIZE.");
    static_assert(alignof(GCObjectHeader) == OBJECT_ALIGNMENT, "Alignment mismatch.");

    inline GCObject* ToObject(GCObjectHeader* header) {
        return reinterpret_cast<GCObject*>(header + 1);
    }

    inline GCObjectHeader* FromObject(GCObject* object) {
        return reinterpret_cast<GCObjectHeader*>(object) - 1;
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_GC_OBJECTHEADER_H