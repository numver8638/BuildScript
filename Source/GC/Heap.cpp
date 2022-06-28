/*
 * Heap.cpp
 * - Allocate and manage garbage collected objects.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/GC/Heap.h>

#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>

#include <BuildScript/GC/Options.h>
#include <BuildScript/GC/Impl/Noop.h>
#include <BuildScript/GC/Handle.h>
#include <BuildScript/GC/Rootable.h>
#include <BuildScript/GC/Tracer.h>

using namespace BuildScript;

using GCCreator = GCAllocator* (*)(const GCOptions&);
struct ThreadGCInfo {
    Internal::GCLocalBase** LocalHandleRoot;
};

// static
GCAllocator* GCHeap::m_impl = nullptr;
static std::unordered_map<std::string_view, GCCreator> GCs = {
    { GCHeap::NoopGC,         &(GCImpl::NoopGC::Create) },
};
static std::mutex ThreadInfoLock;
static std::unordered_map<std::thread::id, ThreadGCInfo> ThreadInfoMap;
static std::mutex RootSetLock;
static std::unordered_set<GCRootable*> RootSet;
extern std::unordered_set<Internal::GCHandleBase*> GlobalHandles; // Defined in Handle.cpp

std::string_view BuildScript::ReasonToString(GCTriggerReason reason) {
    std::string_view strings[] = {
        "allocation failure",
        "requested by user",
    };

    return strings[static_cast<size_t>(reason)];
}

// static
void GCHeap::Initialize(std::string_view name, const GCOptions& options) {
    auto it = GCs.find(name);

    if (it == GCs.end()) {
        // error: GC not found
        return;
    }

    auto* gc = (it->second)(options);

    m_impl = gc;
}

// static
void GCHeap::RegisterThread() {
    std::lock_guard lock(ThreadInfoLock);

    ThreadGCInfo info{};
    info.LocalHandleRoot = &(Internal::GCLocalBase::GetRoot());
    ThreadInfoMap.insert(std::make_pair(std::this_thread::get_id(), info));
}

// static
void GCHeap::UnregisterThread() {
    std::lock_guard lock(ThreadInfoLock);

    auto it = ThreadInfoMap.find(std::this_thread::get_id());

    assert((it != ThreadInfoMap.end()) && "unregistered thread found; all threads must be registered in GCHeap for GC.");

    ThreadInfoMap.erase(it);
}

// static
void GCHeap::AddRoot(GCRootable* rootable) {
    std::lock_guard lock(RootSetLock);
    RootSet.insert(rootable);
}

// static
void GCHeap::RemoveRoot(GCRootable* rootable) {
    std::lock_guard lock(RootSetLock);
    RootSet.erase(rootable);
}

// static
void GCHeap::ScanGCRoots(GCTracer& tracker) {
    // RootSet
    for (auto* rootable : RootSet) {
        rootable->Trace(tracker);
    }

    // GCGlobal
    for (auto* handle : GlobalHandles) {
        tracker.OnVisit(handle->GetReference());
    }

    for (auto& [_, info] : ThreadInfoMap) {
        auto* local_handle = *(info.LocalHandleRoot);
        while (local_handle != nullptr) {
            tracker.OnVisit(local_handle->GetReference());
            local_handle = local_handle->m_prev;
        }
    }
}