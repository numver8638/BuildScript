/*
 * Allocator.h
 * - Implementation interface for garbage collection algorithm.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_ALLOCATOR_H
#define BUILDSCRIPT_GC_ALLOCATOR_H

#include <cstdio>

namespace BuildScript {
    struct GCStatistics;
    struct GCOptions;
    enum class GCFailReason;
    enum class GCRegion;
    enum class GCTriggerReason;

    /**
     * @brief Implementation interface for garbage collection algorithm.
     */
    class GCAllocator {
    protected:
        const GCOptions& m_options;
        std::FILE* m_logfile = nullptr;
        GCFailReason m_reason;

        explicit GCAllocator(const GCOptions&);

    public:
        virtual ~GCAllocator() noexcept;

        virtual void* Allocate(size_t, GCRegion) = 0;

        virtual void Finalize() {}

        virtual GCStatistics GetStatistics() const = 0;

        GCFailReason GetLastFailReason() const { return m_reason; }

        virtual void Collect(int, GCTriggerReason) = 0;

        virtual bool HasPendingGC() const = 0;

        virtual void WaitForGC() = 0;
    }; // end class GCAllocator
} // end namespace BuildScript

#endif // BUILDSCRIPT_GC_ALLOCATOR_H