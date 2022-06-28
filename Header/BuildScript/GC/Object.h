/*
 * Object.h
 * - Represent garbage-collectable objects managed by GCHeap.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_OBJECT_H
#define BUILDSCRIPT_GC_OBJECT_H

#include <cstddef>

#include <BuildScript/Assert.h>
#include <BuildScript/Config.h>

namespace BuildScript {
    class GCTracer;
    class GCHeap;

    /**
     * @brief Represent garbage-collectable objects managed by @a GCHeap.
     * @ref BuildScript::GCHeap
     */
    class EXPORT_API GCObject {
    public:
        virtual ~GCObject() = default;

        /**
         * @brief Callback method when the heap track live objects.
         * @warning All classes that inherit @c GCObject must be implement this method properly otherwise
         *          causes dangling pointer and/or freeing objects that are in use.
         */
        virtual void Trace(GCTracer&) = 0;

        void* operator new(size_t) noexcept = delete;
        void* operator new[](size_t) noexcept = delete;
        void operator delete(void*) { NEVER_BE_CALLED("never call delete explicitly."); }
        void operator delete[](void*) { NEVER_BE_CALLED("never call delete explicitly."); }
    }; // end class GCObject
} // end namespace GCObject

#endif // BUILDSCRIPT_GC_OBJECT_H