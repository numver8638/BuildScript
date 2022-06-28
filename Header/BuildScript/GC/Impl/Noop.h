/*
 * Noop.h
 * - Implementation of allocation only garbage collector.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_IMPL_NOOP_H
#define BUILDSCRIPT_GC_IMPL_NOOP_H

#include <cstdio>

#include <BuildScript/GC/Allocator.h>
#include <BuildScript/Platform/MemoryChunk.h>

namespace BuildScript {
    struct GCOptions;
} // end namespace BuildScript

namespace BuildScript::GCImpl {
    class NoopGC final : GCAllocator {
    private:
        MemoryChunk m_heap, m_metadata;

        void* m_heapTop = nullptr;
        void* m_metadataTop = nullptr;

        size_t m_heapUsed = 0;
        size_t m_metadataUsed = 0;

        NoopGC(const GCOptions& options, size_t heap, size_t metadata);

        void* Allocate(size_t size, GCRegion region) override;

        void Finalize() override;

        GCStatistics GetStatistics() const override;

        void Collect(int generation, GCTriggerReason reason) override;

        bool HasPendingGC() const override;

        void WaitForGC() override;

    public:
        static GCAllocator* Create(const GCOptions& options);
    }; // end class NoopGC
} // end namespace BuildScript::GCImpl

#endif // BUILDSCRIPT_GC_IMPL_NOOP_H