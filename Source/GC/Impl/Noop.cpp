/*
 * Noop.cpp
 * - Implementation of allocation only garbage collector.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/GC/Impl/Noop.h>

#include <BuildScript/GC/Heap.h>
#include <BuildScript/GC/Options.h>

#include <BuildScript/GC/Impl/Shared.h>

using namespace BuildScript;

GCImpl::NoopGC::NoopGC(const GCOptions& options, size_t heap, size_t metadata)
    : GCAllocator(options), m_heap(heap), m_metadata(metadata) {

    m_heapTop = m_heap.GetBase();
    m_metadataTop = m_metadata.GetBase();
}

GCAllocator* GCImpl::NoopGC::Create(const GCOptions& options) {
    return new NoopGC(options, options.MaxPoolSize, options.MaxMetadataSize);
}

void* GCImpl::NoopGC::Allocate(size_t size, GCRegion region) {
    auto alloc_size = CalibrateSize(size);

    void* memory;

    if (region == GCRegion::Metadata) {
        memory = AllocateFromChunk(m_metadata, alloc_size, m_metadataTop, m_metadataUsed);
    }
    else {
        memory = AllocateFromChunk(m_heap, alloc_size, m_heapTop, m_heapUsed);
    }

    if (memory == nullptr) {
        m_reason = GCFailReason::OutOfMemory;
        return nullptr;
    }

    return ToObject(new (memory) GCObjectHeader(size));
}

void GCImpl::NoopGC::Finalize() {

}

GCStatistics GCImpl::NoopGC::GetStatistics() const {
    GCStatistics stat{};

    stat.MaxPoolSize = m_options.MaxPoolSize + m_options.MaxMetadataSize;
    stat.GenMetadataSize = m_options.MaxMetadataSize;
    stat.GenMetadataUsedSize = m_metadataUsed;
    stat.GenOldSize = m_options.MaxPoolSize;
    stat.GenOldUsedSize = m_heapUsed;

    return stat;
}

void GCImpl::NoopGC::Collect(int generation, GCTriggerReason reason) {
    // do nothing.
}

bool GCImpl::NoopGC::HasPendingGC() const {
    return false;
}

void GCImpl::NoopGC::WaitForGC() {
    // do nothing.
}
