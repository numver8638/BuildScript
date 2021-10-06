/*
 * Lexer.h
 * - Token scanner for BuildScript language.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_PARSE_LEXER_H
#define BUILDSCRIPT_COMPILER_PARSE_LEXER_H

#include <string>
#include <vector>

#include <BuildScript/Compiler/Parse/Token.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    class ErrorReporter;    // Defined in <BuildScript/Compiler/ErrorReporter.h>
    class SourceText;       // Defined in <BuildScript/Compiler/SourceText.h>

    enum class ReportID;    // Defined in <BuildScript/Compiler/ErrorReporter.h>

    /**
     * @brief Token scanner for BuildScript language.
     */
    class Lexer final : NonCopyable {
    private:
        SourceText& m_source;
        ErrorReporter& m_reporter;

        // Lexer.cpp
        Token LexInteger(SourcePosition, bool (*)(char32_t), ReportID);
        Token LexNumber(SourcePosition);

        // Lexer.String.cpp
        Token LexString(SourcePosition, char32_t);
        char32_t LexEscape();

        bool SkipInterpolation();

        // Lexer.KeywordTable.cpp
        static TokenType GetKeyword(const std::string& str);

    public:
        /**
         * @brief Construct the lexer.
         * @param source the source text to lex.
         * @param reporter the error reporter.
         */
        Lexer(SourceText& source, ErrorReporter& reporter)
            : m_source(source), m_reporter(reporter) {}

        /**
         * @brief Get next token.
         * @return the token.
         * @see BuildScript::Token
         */
        Token GetNextToken();

        /**
         * @brief Scan interpolations from given range.
         * @param range the range of the string.
         * @return ranges of interpolated expressions in the string.
         */
        std::vector<SourceRange> ScanInterpolations(const SourceRange& range);
    }; // end class Lexer
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_PARSE_LEXER_H