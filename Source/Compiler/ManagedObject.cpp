/*
 * ManagedObject.cpp
 * - Controlled destructor-called object.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/ManagedObject.h>

#include <cstdlib>
#include <iostream>
#include <new>

using namespace BuildScript;

struct alignas(16) BuildScript::ManagedObjectHeader {
    ManagedObjectHeader* Next;
}; // end struct ManagedObjectHeader

inline ManagedObject* ToObject(ManagedObjectHeader* header) {
    // Assume all allocated memories from ManagedObjectAllocator are derived class of ManagedObject.
    return reinterpret_cast<ManagedObject*>(header + 1);
}

void* ManagedObjectAllocator::Allocate(size_t size) {
    void* memory = std::malloc(sizeof(ManagedObjectHeader) + size);

    #pragma warning(disable: 6386)
    auto* header = new (memory) ManagedObjectHeader();

    // Use single linked list to trace allocated objects
    header->Next = m_first;
    m_first = header;

    m_allocated += size;

    return (header + 1);
}

ManagedObjectAllocator::~ManagedObjectAllocator() {
    // deallocate memories
    auto* header = m_first;
    while (header != nullptr) {
        auto* prev = header;
        header = prev->Next;

        ToObject(prev)->~ManagedObject();
        std::free(prev);
    }
}

void ManagedObjectAllocator::Dump() const {
    std::cout << "Total allocated bytes: " << m_allocated << std::endl;
}