/*
 * ErrorReporter.cpp
 * - Collect and notify errors during compile.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/ErrorReporter.h>

using namespace BuildScript;

const struct {
    Severity Severity;
    const char* Message;
} MessageTable[] = {
    #define REPORT_ID(severity, id, message) { severity, u8"" message },
    #include <BuildScript/Compiler/ErrorReporter.ReportID.def>
    #undef REPORT_ID
};

// static
Severity ErrorReporter::GetSeverity(ReportID id) {
    return MessageTable[static_cast<size_t>(id)].Severity;
}

// static
const char* ErrorReporter::GetMessage(ReportID id) {
    return MessageTable[static_cast<size_t>(id)].Message;
}