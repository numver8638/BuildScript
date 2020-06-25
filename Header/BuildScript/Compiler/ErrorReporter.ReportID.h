/*
 * ErrorReporter.ReportID.h
 * - Report and log errors and warnings.
 *   This file is part of ErrorReporter and contains ReportIDs.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ERRORREPOTER_REPORTID_H
#define BUILDSCRIPT_COMPILER_ERRORREPOTER_REPORTID_H

namespace BuildScript {
    enum class ErrorLevel;

    /**
     * @brief Represent the error contents like error message and error level.
     */
    enum class ReportID {
        #define _(id)            id,
        #define ERROR(id, msg)   _(id)
        #define WARNING(id, msg) _(id)
        #define INFO(id, msg)    _(id)

        #include <BuildScript/Compiler/ErrorReporter.ReportID.def>

        #undef _
    }; // end enum ReportID

    /**
     * @brief Get an error message from ReportID.
     * @param id a ReportID.
     * @return an error message.
     */
    EXPORT_API const char* ReportIDToString(ReportID id);

    /**
     * @brief Get a error level from ReportID.
     * @param id a ReportID.
     * @return an ErrorLevel.
     * @see ErrorLevel
     */
    EXPORT_API ErrorLevel ReportIDToErrorLevel(ReportID id);
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ERRORREPOTER_REPORTID_H