/*
 * Rootable.h
 * - Interface for non garbage collected object to track garbage collected object.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_ROOTABLE_H
#define BUILDSCRIPT_GC_ROOTABLE_H

#include <BuildScript/Config.h>

namespace BuildScript {
    class GCTracer;

    /**
     * @brief Interface for non garbage collected object to track garbage collected object.
     */
    class EXPORT_API GCRootable {
    protected:
        GCRootable();

    public:
        virtual ~GCRootable() noexcept;
        virtual void Trace(GCTracer&) = 0;
    }; // end class GCRootable
} // end namespace BuildScript

#endif // BUILDSCRIPT_GC_ROOTABLE_H