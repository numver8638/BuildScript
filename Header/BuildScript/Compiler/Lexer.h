/*
 * Lexer.h
 * - Semantic analyzer (a.k.a lexer) for BuildScript language.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_LEXER_H
#define BUILDSCRIPT_COMPILER_LEXER_H

#include <tuple>
#include <vector>

#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/Token.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    class SourceText; // Defined in <BuildScript/Compiler/SourceText.h>

    /**
     * @brief State of lexer.
     */
    enum class LexerMode {
        Default,            //!< Basic mode for lexer.
        String,             //!< Accept characters as string, not other token.
        InterpolateBegin,   //!< Beginning of interpolated expression.
        Interpolate,        //!< Similar Default mode but in string.
    }; // end enum LexerMode

    /**
     * @brief Semantic analyzer (a.k.a lexer) for BuildScript language.
     */
    class EXPORT_API Lexer : NonCopyable {
    private:
        SourceText& m_source;
        ErrorReporter& m_reporter;
        LexerMode m_mode = LexerMode::Default;
        std::vector<std::tuple<LexerMode, int>> m_states;
        std::vector<bool> m_depth;

        void HandleEscapeSequence();

        Token LexTokenInternal();
        Token LexNumber();
        Token LexIdentifier();
        Token LexString(int);
        Token LexPunctuator();
        Token LexComment();
        Token LexInterpolateBegin();

        Token Invalid(ReportID id);

    public:
        /**
         * @brief Constructor for Lexer.
         * @param source a source text to lex.
         * @param reporter a reporter for reporting errors.
         */
        Lexer(SourceText& source, ErrorReporter& reporter);
        
        /**
         * @brief Tokenize text.
         * @return a token.
         */
        Token LexToken();
    }; // end class Lexer
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_LEXER_H