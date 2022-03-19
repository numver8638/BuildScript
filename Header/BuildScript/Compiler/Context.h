/*
 * Context.h
 * - Shared data during compile.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_CONTEXT_H
#define BUILDSCRIPT_COMPILER_CONTEXT_H

#include <cstddef>

#include <BuildScript/Compiler/ManagedObject.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    struct CompileOptions; // Defined in <BuildScript/Compiler/CompileOptions.h>
    class ErrorReporter; // Defined in <BuildScript/Compiler/ErrorReporter.h>
    class SourceText; // Defined in <BuildScript/Compiler/SourceText.h>

    /**
     * @brief Shared data during compile.
     */
    class Context final : NonCopyable {
    private:
        ManagedObjectAllocator m_allocator;

        const CompileOptions& m_options;
        SourceText& m_source;
        ErrorReporter& m_reporter;

    public:
        Context(const CompileOptions& options, SourceText& source, ErrorReporter& reporter)
            : m_options(options), m_source(source), m_reporter(reporter) {}

        /**
         * @brief Get an allocator.
         * @return a reference of @c ManagedObjectAllocator.
         */
        ManagedObjectAllocator& GetAllocator() { return m_allocator; }

        /**
         * @brief Get an options of current compile.
         * @return readonly reference of @c CompileOptions.
         */
        const CompileOptions& GetOptions() const { return m_options; }

        /**
         * @brief Get current compiling source text.
         * @return a reference of @c SourceText.
         */
        SourceText& GetSource() { return m_source; }

        /**
         * @brief Get an reporter.
         * @return a reference of @c ErrorReporter.
         */
        ErrorReporter& GetReporter() { return m_reporter; }
    }; // end class Context
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_CONTEXT_H