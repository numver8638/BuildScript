/*
 * ErrorReporter.ReportID.h
 * - Collect and notify errors during compile.
 *   This file is part of ErrorReporter.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ERRORREPORTER_REPORTID_H
#define BUILDSCRIPT_COMPILER_ERRORREPORTER_REPORTID_H

namespace BuildScript {
    /**
     * @brief Report ID.
     */
    enum class ReportID {
        #define REPORT_ID(severity, id, message) id,
        #include <BuildScript/Compiler/ErrorReporter.ReportID.def>
        #undef REPORT_ID
    }; // end enum ReportID
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ERRORREPORTER_REPORTID_H