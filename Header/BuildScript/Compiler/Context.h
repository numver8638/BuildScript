/*
 * Context.h
 * - Shared data during compile.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_CONTEXT_H
#define BUILDSCRIPT_COMPILER_CONTEXT_H

#include <cstddef>

#include <BuildScript/Compiler/ManagedObject.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    /**
     * @brief Shared data during compile.
     */
    class Context final : NonCopyable {
    private:
        ManagedObjectAllocator m_allocator;

    public:
        ManagedObjectAllocator& GetAllocator() { return m_allocator; }
    }; // end class Context
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_CONTEXT_H