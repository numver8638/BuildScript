/*
 * Heap.h
 * - Allocate and manage garbage collected objects.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_HEAP_H
#define BUILDSCRIPT_GC_HEAP_H

#include <cstddef>
#include <string_view>
#include <new>

#include <BuildScript/Config.h>
#include <BuildScript/GC/Object.h>
#include <BuildScript/GC/Allocator.h>
#include <BuildScript/Utils/AllStatic.h>

namespace BuildScript {
    struct GCOptions;
    class GCRootable;

    /**
     * @brief Statistics of GC.
     */
    struct GCStatistics {
        int MinorGCCount;               //!< Count of minor GC.
        int MajorGCCount;               //!< Count of major GC.
        size_t MaxPoolSize;             //!< Total size of GC heap. Total = (Nursery + Survivor + Old + Metadata)
        size_t GenNurserySize;          //!< Size of nursery area. May be zero if the algorithm not support it.
        size_t GenNurseryUsedSize;      //!< Used size of nursery area.
        size_t GenSurvivorSize;         //!< Size of survivor area. May be zero if the algorithm not support it.
        size_t GenSurvivorUsedSize;     //!< Used size of survivor area.
        size_t GenOldSize;              //!< Size of old area. This field may represent size of heap
                                        //!< if the algorithm does not support generational algorithm.
        size_t GenOldUsedSize;          //!< Used size of old area.
        size_t GenMetadataSize;         //!< Size of metadata area.
        size_t GenMetadataUsedSize;     //!< Used size of metadata area.
    }; // end struct GCStatistics

    /**
     * @brief Represents an region where the memory allocated.
     */
    enum class GCRegion {
        Heap,       //!< Allocated from the normal heap.
        Metadata,   //!< Allocated from the metadata area.
    }; // end enum GCRegion

    /**
     * @brief Describes why garbage collection is failed.
     */
    enum class GCFailReason {
        None,               //!< GC was successful.
        OutOfMemory,        //!< Memories are exhausted.
        OSError,            //!< Failed gather memory from OS.
        HeapCorruption      //!< Heap was corrupted.
    }; // end enum GCFailReason

    /**
     * @brief Describes why garbage collection is triggered.
     */
    enum class GCTriggerReason {
        AllocationFailure,  //!< GC was triggered due to out of memory.
        RequestedByUser,    //!< GC was triggered by user.
    }; // end enum GCTriggerReason

    std::string_view ReasonToString(GCTriggerReason);

    /**
     * @brief Allocate and manage garbage collected objects.
     */
    class EXPORT_API GCHeap final : AllStatic {
    private:
        static GCAllocator* m_impl;

    public:
        /**
         * @brief Key for do-nothing(do allocation only, fail when exhausted) garbage collection algorithm.
         */
        static constexpr std::string_view NoopGC = "noop";

        /**
         * @brief Initialize the heap.
         * @param key a key of the algorithm.
         * @param options a @c GCOption applied for the heap.
         */
        static void Initialize(std::string_view key, const GCOptions& options);

        /**
         * @brief Finalize the heap.
         */
        static void Finalize() {
            m_impl->Finalize();
            delete m_impl;
        }

        /**
         * @brief Register current thread from the heap.
         * @note Every newly created threads are must be call this method before any usage of GC APIs and/or APIs that
         *       uses GC API to gather root set and collect garbage properly.
         */
        static void RegisterThread();

        /**
         * @brief Unregister current thread from the heap.
         * @note Every threads are must be call this method before terminating itself.
         */
        static void UnregisterThread();

        /**
         * @brief Add @c GCRootable to the heap's root set.
         * @note @c GCRootable is self-registered when itself constructs. No need for calling the method manually.
         */
        static void AddRoot(GCRootable*);

        /**
         * @brief Remove @c GCRootable to the heap's root set.
         * @note @c GCRootable is self-unregistered when itself destructs. No need for calling the method manually.
         */
        static void RemoveRoot(GCRootable*);

        /**
         * @brief Iterate all @c GCObject in the root set.
         * @note This method is exposed for GC algorithm implementations.
         */
        static void ScanGCRoots(GCTracer&);

        /**
         * @brief Allocate memory within given region.
         * @param size size of memory to allocate.
         * @param region @c GCRegion where allocate memory.
         * @return non-null pointer if succeed, otherwise @c nullptr.
         *         Check @c GetLastFailReason() why the heap fail to allocate memory.
         * @see GCHeap::GetLastFailReason()
         */
        static void* Allocate(size_t size, GCRegion region = GCRegion::Heap) { return m_impl->Allocate(size, region); }

        /**
         * @brief Create new object. All objects of class that inherit @c GCObject can be created via this method.
         * @tparam T a type of the object to create.
         * @tparam Args types of parameters.
         * @param args arguments the type @c T requires
         * @return Freshly created object of @c T or @c nullptr if fail to allocate memory.
         *         Check @c GetLastFailReason() why the heap fail to allocate memory.
         * @see GCHeap::GetLastFailReason()
         */
        template <typename T, typename... Args>
        static T* New(Args&&... args) {
            static_assert(std::is_base_of_v<GCObject, T>, "T is not base of GCObject.");

            void* mem = Allocate(sizeof(T));

            if (mem == nullptr) { return nullptr; }

            T* obj = ::new (mem) T(std::forward<Args&&>(args)...);
            return obj;
        }

        /**
         * @brief Get statistical information of the heap.
         * @return @c GCStatistics contains statistics of the heap.
         * @see BuildScript::GCStatistics
         */
        static GCStatistics GetStatistics() { return m_impl->GetStatistics(); }

        /**
         * @brief Get latest reason why allocation failed.
         * @return @c GCFailReason describing allocation fail reason.
         * @see BuildScript::GCFailReason
         */
        static GCFailReason GetLastFailReason() { return m_impl->GetLastFailReason(); }

        /**
         * @brief Do garbage collection.
         * @param generation generation number which generation is collected. May be ignored if the algorithm do not
         *                   support generational garbage collection algorithm.
         */
        static void Collect(int generation = 0) { m_impl->Collect(generation, GCTriggerReason::RequestedByUser); }
    }; // end class GCHeap
} // end namespace BuildScript

#endif // BUILDSCRIPT_GC_HEAP_H