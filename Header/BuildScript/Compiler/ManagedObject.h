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

#include <cstddef>

#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    struct ManagedObjectHeader;

    /**
     * @brief Manage allocations and trace objects allocated during compile.
     *        All objects inherit @c ManagedObject are freed on destruction of @c ManagedObjectAllocator.
     *
     * @see BuildScript::ManagedObject
     */
    class ManagedObjectAllocator final : NonCopyable {
        friend class ManagedObject;

    private:
        ManagedObjectHeader* m_first = nullptr;
        size_t m_allocated = 0;

        [[nodiscard]]
        void* Allocate(size_t);

    public:
        ~ManagedObjectAllocator();

        void Dump() const;
    }; // end class ManagedObjectAllocator

    /**
     * @brief Controlled destructor-called object.
     *        All classes inherit this class are allocated and traced by @c Context.
     *
     * @see BuildScript::ManagedObjectAllocator
     */
    class ManagedObject {
    public:
        virtual ~ManagedObject() = default;

        [[nodiscard]]
        void* operator new(size_t size, ManagedObjectAllocator& allocator) { return allocator.Allocate(size); }
        void operator delete(void*, ManagedObjectAllocator&) noexcept {}

        [[nodiscard]]
        void* operator new(size_t size, ManagedObjectAllocator& allocator, size_t additional) {
            return allocator.Allocate(size + additional);
        }
        void operator delete(void*, ManagedObjectAllocator&, size_t) noexcept {}

        void* operator new(size_t) = delete;
        void operator delete(void*, size_t) noexcept {}
    }; // end class ManagedObject
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_MANAGEDOBJECT_H