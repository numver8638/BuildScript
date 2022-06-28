/*
 * MemoryPool.cpp
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Platform/MemoryChunk.h>

#include <cassert>

using namespace BuildScript;

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void* OSReserveMemory(size_t size) {
    return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
}

void OSCommitMemory(void* base, size_t size) {
    VirtualAlloc(base, size, MEM_COMMIT, PAGE_READWRITE);
}

void OSReleaseMemory(void* base, size_t size) {
    VirtualFree(base, size, MEM_DECOMMIT);
}

void OSFreeMemory(void* base, size_t size) {
    VirtualFree(base, size, MEM_RELEASE);
}

size_t OSGetPageSize() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    return info.dwPageSize;
}

#else // !OS_WINDOWS
#include <sys/mman.h>
#include <unistd.h>

void* OSReserveMemory(size_t size) {
    void* ptr = mmap(nullptr, size, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    return ptr;
}

void OSCommitMemory(void* base, size_t size) {
    mprotect(base, size, PROT_READ|PROT_WRITE);
}

void OSReleaseMemory(void* base, size_t size) {
    mprotect(base, size, PROT_NONE);
}

void OSFreeMemory(void* base, size_t size) {
    munmap(base, size);
}

size_t OSGetPageSize() {
    return sysconf(_SC_PAGESIZE);
}
#endif

static size_t PageSize = OSGetPageSize();

MemoryChunk::MemoryChunk(size_t size) : m_size(size) {
    assert((size % PageSize) == 0 && "not page aligned.");

    m_base = OSReserveMemory(size);
}

MemoryChunk::~MemoryChunk() noexcept {
    OSFreeMemory(m_base, m_size);
}

void MemoryChunk::Commit(size_t size) {
    assert((m_commitSize + size) <= m_size);
    assert((size % PageSize) == 0 && "not page aligned.");

    void* base = reinterpret_cast<std::byte*>(m_base) + m_commitSize;

    OSCommitMemory(base, size);

    m_commitSize += size;
}

void MemoryChunk::Release(size_t size) {
    assert((size % PageSize) == 0 && "not page aligned.");

    void* base = reinterpret_cast<std::byte*>(m_base) + (m_commitSize - size);

    assert(m_base <= base);

    OSReleaseMemory(base, size);

    m_commitSize -= size;
}

// static
size_t MemoryChunk::GetPageSize() { return PageSize; }