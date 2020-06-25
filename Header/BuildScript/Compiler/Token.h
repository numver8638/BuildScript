/*
 * Token.h
 * - Represent a lexme in a script.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_TOKEN_H
#define BUILDSCRIPT_COMPILER_TOKEN_H

#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Compiler/StringRef.h>

namespace BuildScript {
    /**
     * @brief Define a type of token.
     */
    enum class TokenType {
        Invalid,

        EndOfFile,
        EndOfLine,
        Comment,           // # ...

        Identifier,        // id
        Integer,           // 1234
        Float,             // 12.34
        String,            // "" or ''
        InterpolateBegin,  // ${
        InterpolateEnd,    // }
        InterpolatedVar,   // $id

        // Keywords
        And,               // and
        As,                // as
        Assert,            // assert
        Break,             // break
        Case,              // case
        Class,             // class
        Continue,          // continue
        Def,               // def
        Default,           // default
        Defined,           // defined
        Deinit,            // deinit
        DependsOn,         // dependsOn
        Do,                // do
        DoFirst,           // doFirst
        DoLast,            // doLast
        Else,              // else
        Except,            // except
        Export,            // export
        Extends,           // extends
        False,             // false
        Finally,           // finally
        For,               // for
        From,              // from
        Get,               // get
        If,                // if
        Import,            // import
        In,                // in
        Init,              // init
        Inputs,            // inputs
        Is,                // is
        Match,             // match
        None,              // none
        Not,               // not
        Operator,          // operator
        Or,                // or
        Outputs,           // outputs
        Pass,              // pass
        Raise,             // raise
        Return,            // return
        Self,              // self
        Set,               // set
        Static,            // static
        Super,             // super
        Task,              // task
        True,              // true
        Try,               // try
        Var,               // var
        While,             // while
        With,              // with

        // Punctuators
        PunctuatorBegin,
        Add = PunctuatorBegin, // +
        InplaceAdd,        // +=
        Sub,               // -
        InplaceSub,        // -=
        Mul,               // *
        InplaceMul,        // *=
        Div,               // /
        InplaceDiv,        // /=
        Mod,               // %
        InplaceMod,        // %=
        BitAnd,            // &
        InplaceBitAnd,     // &=
        BitOr,             // |
        InplaceBitOr,      // |=
        BitNot,            // ~
        BitXor,            // ^
        InplaceBitXor,     // ^=
        Less,              // <
        LessOrEqual,       // <=
        LeftShift,         // <<
        InplaceLeftShift,  // <<=
        Grater,            // >
        GraterOrEqual,     // >=
        RightShift,        // >>
        InplaceRightShift, // >>=
        Equal,             // ==
        NotEqual,          // !=
        Assign,            // =
        Comma,             // ,
        Colon,             // :
        Dot,               // .
        Ellipsis,          // ...
        Arrow,             // =>
        LeftParen,         // (
        RightParen,        // )
        LeftBrace,         // {
        RightBrace,        // }
        LeftSquare,        // [
        RightSquare,       // ]
    }; // end enum TokenType

    /**
     * @brief Represents a lexme in a script.
     */
    struct Token {
        TokenType Type;           //!< .
        SourcePosition Position;  //!< .
        StringRef Image;          //!< .

        /**
         * @brief Default constructor.
         */
        Token() = default;

        /**
         * @brief Construct the token.
         * @param type a type of the token.
         * @param pos a position where token starts.
         */
        Token(TokenType type, const SourcePosition& pos)
            : Type(type), Position(pos) {}

        /**
         * @brief Construct the token.
         * @param type a type of the token.
         * @param pos a position where token starts.
         * @param image a 
         */
        Token(TokenType type, const SourcePosition& pos, const StringRef& image)
            : Type(type), Position(pos), Image(image) {}

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
         * @param type a type of the token.
         * @return true if a type of the token equals, otherwise false.
         */
        bool operator ==(TokenType type) const { return Type == type; }

        /**
         * @brief Test if a type of the token is not equal to right.
         * @param type a type of the token.
         * @return true if a type of the token doesn't equal, otherwise false.
         */
        bool operator !=(TokenType type) const { return Type != type; }
    }; // end struct Token
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_TOKEN_H