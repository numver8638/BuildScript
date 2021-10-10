/*
 * ManagedObject.h
 * - Controlled destructor-called object.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_MANAGEDOBJECT_H
#define BUILDSCRIPT_COMPILER_MANAGEDOBJECT_H

#include <BuildScript/Compiler/Context.h>

namespace BuildScript {
    /**
     * @brief Controlled destructor-called object.
     *        All classes inherit this class are allocated and traced by @c Context.
     *
     * @see BuildScript::Context
     */
    class ManagedObject {
    public:
        virtual ~ManagedObject() = default;

        [[nodiscard]]
        void* operator new(size_t size, Context& context) { return context.Allocate(size); }
        void operator delete(void*, Context&) noexcept {}

        void* operator new(size_t) = delete;
        void operator delete(void*, size_t) noexcept {}
    }; // end class ManagedObject
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_MANAGEDOBJECT_H