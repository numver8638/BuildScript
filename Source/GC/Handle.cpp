/*
 * Handle.cpp
 * - Reference of garbage collected object.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/GC/Handle.h>

#include <mutex>
#include <unordered_set>

using namespace BuildScript;

static std::mutex GlobalHandleMapLock;
std::unordered_set<Internal::GCHandleBase*> GlobalHandles;

void Internal::GCHandleBase::RegisterGlobal(Internal::GCHandleBase* handle) {
    std::lock_guard lock(GlobalHandleMapLock);
    GlobalHandles.emplace(handle);
}

void Internal::GCHandleBase::UnregisterGlobal(Internal::GCHandleBase* handle) {
    std::lock_guard lock(GlobalHandleMapLock);
    GlobalHandles.erase(handle);
}

void Internal::GCHandleBase::WriteBarrier(Internal::GCHandleBase* handle, GCObject* value) {
    // TODO: Implement write barrier
    handle->m_ptr = value;
}

static thread_local Internal::GCLocalBase* HandleRoot = nullptr;

// static
Internal::GCLocalBase*& Internal::GCLocalBase::GetRoot() {
    return HandleRoot;
}