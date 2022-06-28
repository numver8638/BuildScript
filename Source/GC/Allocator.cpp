/*
 * GCAllocator.cpp
 * - Implementation interface for garbage collection algorithm.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/GC/Allocator.h>

#include <BuildScript/GC/Heap.h>
#include <BuildScript/GC/Options.h>

#ifdef OS_WINDOWS
#define OPEN_FILE(handle, filename, mode) \
        (fopen_s(&(handle), (filename), (mode)))
#else
#define OPEN_FILE(handle, filename, mode) \
        ((handle) = std::fopen((filename), (mode)))
#endif // OS_WINDOWS

using namespace BuildScript;

GCAllocator::GCAllocator(const GCOptions& options)
    : m_options(options), m_reason(GCFailReason::None) {

    if (options.EnableGCLog) {
        OPEN_FILE(m_logfile, options.GCLogPath.c_str(), "w");
    }
}

GCAllocator::~GCAllocator() noexcept {
    if (m_logfile != nullptr) {
        std::fclose(m_logfile);
    }
}