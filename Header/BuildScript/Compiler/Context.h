/*
 * Context.h
 * - Manage allocations and trace objects allocated during compile.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_CONTEXT_H
#define BUILDSCRIPT_COMPILER_CONTEXT_H

#include <cstddef>

#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    struct ManagedObjectHeader;

    /**
     * @brief Manage allocations and trace objects allocated during compile.
     *        All objects inherit @c ManagedObject are freed on destruction of @c Context.
     *
     * @see BuildScript::ManagedObject
     */
    class Context final : NonCopyable {
        friend class ManagedObject;

        ManagedObjectHeader* m_first = nullptr;
        size_t m_allocated = 0;

    private:
        [[nodiscard]]
        void* Allocate(size_t);

    public:
        ~Context();

        /**
         * @brief Dump memory usage to stdout.
         */
        void Dump() const;
    }; // end class Context
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_CONTEXT_H