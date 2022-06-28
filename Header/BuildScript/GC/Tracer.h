/*
 * Tracer.h
 * - Trace garbage collected objects.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_TRACER_H
#define BUILDSCRIPT_GC_TRACER_H

#include <BuildScript/GC/Handle.h>
#include <BuildScript/GC/ValueHandle.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    /**
     * @brief Trace garbage collected objects.
     */
    class GCTracer : NonCopyable {
        friend class GCHeap;

    protected:
        virtual void OnVisit(GCObjectRef) = 0;

    public:
        virtual ~GCTracer() noexcept = default;

        /**
         * @brief Trace a handle.
         * @tparam T
         * @param handle
         */
        template <typename T>
        void Trace(GCGlobal<T>& handle) {
            OnVisit(handle.GetReference());
        }

        /**
         * @brief Trace a handle.
         * @tparam T
         * @param handle
         */
        template <typename T>
        void Trace(GCLocal<T>& handle) {
            OnVisit(handle.GetReference());
        }

        /**
         * @brief Trace a handle.
         * @tparam T
         * @param handle
         */
        template <typename T>
        void Trace(GCMember<T>& handle) {
            OnVisit(handle.GetReference());
        }

        /**
         * @brief Trace a handle.
         * @param handle
         */
        void Trace(ValueHandle& handle) {
            if (handle.m_value.GetType() == ValueType::Object) {
                OnVisit(handle.m_value.AsReference());
            }
        }

        /**
         * @brief Trace raw value.
         * @param value
         */
        void Trace(ScriptValue &value) {
            if (value.GetType() == ValueType::Object) {
                OnVisit(value.AsReference());
            }
        }
    }; // end class GCTracer
} // end namespace BuildScript

#endif // BUILDSCRIPT_GC_TRACER_H