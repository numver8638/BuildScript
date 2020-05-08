/*
 * ErrorReporter.ReportID.cpp
 * - Report and log errors and warnings.
 *   This file is part of ErrorReporter and contains error messages correspond to ReportID.
 *   
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/ErrorReporter.h>

#include <cstddef>

using namespace BuildScript;

static const struct {
    const char* Message;
    ErrorLevel ErrorLevel;
} Table[] = {
    #define _(msg, level)    { msg, level },
    #define ERROR(id, msg)   _(msg, ErrorLevel::Error)
    #define WARNING(id, msg) _(msg, ErrorLevel::Warning)
    #define INFO(id, msg)    _(msg, ErrorLevel::Info)
    #include <BuildScript/Compiler/ErrorReporter.ReportID.def>
};

ErrorLevel BuildScript::ReportIDToErrorLevel(ReportID id) {
    return Table[static_cast<size_t>(id)].ErrorLevel;
}

const char* BuildScript::ReportIDToString(ReportID id) {
    return Table[static_cast<size_t>(id)].Message;
}