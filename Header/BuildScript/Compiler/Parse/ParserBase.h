/*
 * ParserBase.h
 * - Base class of the parser.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */

#ifndef BUILDSCRIPT_COMPILER_PARSE_PARSERBASE_H
#define BUILDSCRIPT_COMPILER_PARSE_PARSERBASE_H

#include <list>
#include <array>

#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Compiler/Parse/Lexer.h>
#include <BuildScript/Compiler/Parse/Token.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    class ErrorReporter;    // Defined in <BuildScript/Compiler/Basic/ErrorReporter.h>
    class SourceText;       // Defined in <BuildScript/Compiler/Basic/SourceText.h>

    /**
     * @brief Base class of the parser.
     *
     * This class implements basic functionality of the parser.
     */
    class ParserBase : NonCopyable {
    protected:
        SourceText& m_source;
        ErrorReporter& m_reporter;

        Lexer m_lexer;
        Token m_token;
        SourceRange m_prevRange;

    private:
        bool m_newline = false;
        std::list<Token> m_buffer;

        void Consume();

    protected:
        /**
         * @brief Create @c ParserBase.
         * @param source the source text to parse.
         * @param reporter the reporter to report errors.
         */
        ParserBase(SourceText& source, ErrorReporter& reporter);

        /**
         * @brief Peek a token.
         * @param lookahead
         * @return a reference of peeked token.
         */
        const Token& PeekToken(int lookahead = 0);

        /**
         * @brief Consume current token and advance to next one.
         * @return a position of consumed token.
         */
        SourcePosition ConsumeToken() {
            auto pos = m_token.GetPosition();
            Consume();
            return pos;
        }

        /**
         * @brief Consume token, returning @c SourceRange rather than @c SourcePosition.
         * @return a range of consumed token.
         */
        SourceRange ConsumeTokenRange() {
            auto range = m_token.Range;
            Consume();
            return range;
        }

        /**
         * @brief Consume token if current one is desired one.
         * @param desired the type of desired token.
         * @param out a position of consumed token. Not modified when token is not desired one.
         * @return boolean value whether token is consumed or not.
         */
        bool ConsumeIf(TokenType desired, SourcePosition& out) {
            auto matched = (m_token == desired);

            if (matched) {
                out = ConsumeToken();
            }

            return matched;
        }

        /**
         * @brief Check there was a newline before current token.
         * @return @c true if there was a newline, otherwise @c false.
         */
        bool HasEOL() const { return m_newline; }

        /**
         * @brief
         * @tparam Type
         * @return
         */
        template <TokenType Type>
        bool OneOf() const { return m_token == Type; }

        template <>
        bool OneOf<TokenType::EndOfLine>() const { return HasEOL(); }

        template <TokenType Type1, TokenType Type2, TokenType... Types>
        bool OneOf() const {
            return OneOf<Type1>() || OneOf<Type2, Types...>();
        }

        /**
         * @brief Skip tokens that are matched type in @c Types.
         * @tparam Types list of @c TokenType
         */
        template <TokenType... Types>
        void SkipUntil() {
            while (!OneOf<Types...>()) {
                Consume();
            }
        }

        /**
         * @brief Assert current token is expected type and consume or report error if current one is not expected one.
         * @param expected expected type of the token.
         * @return The position of consumed token or the position of unexpected token if current token is
         *         not expected one.
         */
        SourcePosition RequireToken(TokenType expected);

        /**
         * @brief Assert current token is an identifier and consume or report error if current one is not an identifier.
         * @return @c RangedString if current token is an identifier otherwise empty one.
         */
        Identifier RequireIdentifier();

        /**
         * @brief Assert there's an EOL(end of line) and report error if not exists.
         */
        void RequireEOL();
    }; // end class ParserBase
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_PARSE_PARSERBASE_H