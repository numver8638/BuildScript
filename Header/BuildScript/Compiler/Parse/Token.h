/*
 * Token.h
 * - Represent a lexeme in a script.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_PARSE_TOKEN_H
#define BUILDSCRIPT_COMPILER_PARSE_TOKEN_H

#include <cassert>
#include <string>

#include <BuildScript/Compiler/SourcePosition.h>

namespace BuildScript {
    /**
     * @brief Define a type of token.
     */
    enum class TokenType {
        Invalid,

        EndOfFile,
        EndOfLine,
        Comment,                // # ...

        Identifier,             // id
        Integer,                // 1234
        Float,                  // 12.34
        String,                 // "" or ''

        // Keywords
        And,                    // and
        As,                     // as
        Assert,                 // assert
        Break,                  // break
        Case,                   // case
        Class,                  // class
        Continue,               // continue
        Const,                  // const
        Def,                    // def
        Default,                // default
        Defined,                // defined
        Deinit,                 // deinit
        DependsOn,              // dependsOn
        Do,                     // do
        DoFirst,                // doFirst
        DoLast,                 // doLast
        Else,                   // else
        Except,                 // except
        Export,                 // export
        Extends,                // extends
        False,                  // false
        Finally,                // finally
        For,                    // for
        From,                   // from
        Get,                    // get
        If,                     // if
        Import,                 // import
        In,                     // in
        Init,                   // init
        Inputs,                 // inputs
        Is,                     // is
        Match,                  // match
        None,                   // none
        Not,                    // not
        Operator,               // operator
        Or,                     // or
        Outputs,                // outputs
        Pass,                   // pass
        Raise,                  // raise
        Return,                 // return
        Self,                   // self
        Set,                    // set
        Static,                 // static
        Super,                  // super
        Task,                   // task
        True,                   // true
        Try,                    // try
        Var,                    // var
        While,                  // while
        With,                   // with

        // Punctuators
        Add,                    // +
        Sub,                    // -
        Mul,                    // *
        Div,                    // /
        Mod,                    // %
        LeftShift,              // <<
        RightShift,             // >>
        BitAnd,                 // &
        BitOr,                  // |
        BitNot,                 // ~
        BitXor,                 // ^
        Less,                   // <
        LessOrEqual,            // <=
        Grater,                 // >
        GraterOrEqual,          // >=
        Equal,                  // ==
        NotEqual,               // !=
        Assign,                 // =
        InplaceAdd,             // +=
        InplaceSub,             // -=
        InplaceMul,             // *=
        InplaceDiv,             // /=
        InplaceMod,             // %=
        InplaceBitAnd,          // &=
        InplaceBitOr,           // |=
        InplaceBitXor,          // ^=
        InplaceLeftShift,       // <<=
        InplaceRightShift,      // >>=
        Comma,                  // ,
        Colon,                  // :
        Dot,                    // .
        Ellipsis,               // ...
        Arrow,                  // =>
        LeftParen,              // (
        RightParen,             // )
        LeftBrace,              // {
        RightBrace,             // }
        LeftSquare,             // [
        RightSquare,            // ]
    }; // end enum TokenType

    /**
     * @brief Represents lexeme in a script.
     */
    struct Token {
        TokenType   Type = TokenType::Invalid;  //!< The type of the token.
        SourceRange Range;                      //!< The range of the token.
        std::string Image;                      //!< The text of the token. Valid when token is identifier or string.

        /**
         * @brief Default constructor.
         */
        Token() = default;

        /**
         * @brief Construct the token.
         * @param type a type of the token.
         * @param range a range of the token.
         */
        Token(TokenType type, const SourceRange& range)
            : Type(type), Range(range) {}

        /**
         * @brief Construct the token.
         * @param type a type of the token.
         * @param range a range of the token.
         * @param image a text of the token.
         */
        Token(TokenType type, const SourceRange& range, std::string image)
            : Type(type), Range(range), Image(std::move(image)) {}

        /**
         * @brief Copy construct with other token.
         * @param rhs other token to copy.
         */
        Token(const Token& rhs) = default;

        /**
         * @brief Copy-assign other token.
         * @param rhs other token to assign.
         * @return the token itself.
         */
        Token& operator =(const Token& rhs) = default;

        /**
         * @brief Test if a type of the token is equal to right.
         * @param rhs a type of the token.
         * @return true if a type of the token equals, otherwise false.
         */
        bool operator ==(TokenType rhs) const { return Type == rhs; }

        /**
         * @brief Test if a type of the token is not equal to right.
         * @param rhs a type of the token.
         * @return true if a type of the token doesn't equal, otherwise false.
         */
        bool operator !=(TokenType rhs) const { return Type != rhs; }

        /**
         * @brief Get a position of the token.
         * @return a position of the token.
         */
        SourcePosition GetPosition() const { return Range.Begin; }

        /**
         * @brief Stringize the token's type.
         * @return stringized @c TokenType.
         * @see BuildScript::Token::TypeToString(BuildScript::TokenType)
         */
        const char* TypeToString() const { return TypeToString(Type); }

        /**
         * @brief Stringize given @c TokenType.
         * @param type the @c TokenType.
         * @return stringized @c TokenType.
         */
        static const char* TypeToString(TokenType type);
    }; // end struct Token
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_PARSE_TOKEN_H