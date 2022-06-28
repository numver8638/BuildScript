/*
 * Options.h
 * - Options for the GC heap.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_GC_OPTIONS_H
#define BUILDSCRIPT_GC_OPTIONS_H

#include <cstddef>
#include <string>

#include <BuildScript/Utils/Literals.h>

namespace BuildScript {
    /**
     * @brief Options for the GC heap.
     */
    struct GCOptions {
        size_t MaxPoolSize = 128_MB;            //!< Max garbage collected heap size.
        size_t MaxMetadataSize = 32_MB;         //!< Max metadata region size.
        bool EnableGCLog = false;               //!< Enable gc log.
        std::string GCLogPath = "gc.log";       //!< Path where to write gc log. Ignored if @c EnableGCLog is @c false.
        int NewOldGenerationRatio = 1;          //!< Ratio between new and old generation.
        int NurserySurvivorGenerationRatio = 3; //!< Ratio between nursery and survivor generation.
    }; // end struct GCOptions
} // end namespace BuildScript

#endif // BUILDSCRIPT_GC_OPTIONS_H