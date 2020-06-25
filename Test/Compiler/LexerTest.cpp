/*
 * LexerTest.cpp
 * - Test for BuildScript::Lexer.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <iostream>

#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/Lexer.h>
#include <BuildScript/Compiler/SourceText.h>
#include <BuildScript/Compiler/StringRef.h>

using namespace BuildScript;

const struct TokenData {
    TokenType Type;
    unsigned Line;
    unsigned Column;
    const char* Image;
} Expectations[] = {
    #define TOKEN(type, line, column, image) { TokenType::type, line, column, image },
    #include "../Scripts/LexerTest.script.expected"
};

const char* TypeToString(TokenType type) {
    static const char* const StringTable[] = {
        u8"Invalid",

        u8"EndOfFile",
        u8"EndOfLine",
        u8"Comment",

        u8"Identifier",
        u8"Integer",
        u8"Float",
        u8"String",
        u8"InterpolateBegin",
        u8"InterpolateEnd",
        u8"InterpolatedVar",

        // Keywords
        u8"And",
        u8"As",
        u8"Assert",
        u8"Case",
        u8"Class",
        u8"Continue",
        u8"Def",
        u8"Default",
        u8"Defined",
        u8"Deinit",
        u8"DependsOn",
        u8"Do",
        u8"DoFirst",
        u8"DoLast",
        u8"Break",
        u8"Else",
        u8"Except",
        u8"Export",
        u8"Extends",
        u8"False",
        u8"Finally",
        u8"For",
        u8"From",
        u8"Get",
        u8"If",
        u8"Import",
        u8"In",
        u8"Init",
        u8"Inputs",
        u8"Is",
        u8"Match",
        u8"None",
        u8"Not",
        u8"Operator",
        u8"Or",
        u8"Outputs",
        u8"Pass",
        u8"Raise",
        u8"Return",
        u8"Self",
        u8"Set",
        u8"Static",
        u8"Super",
        u8"Task",
        u8"True",
        u8"Try",
        u8"Var",
        u8"While",
        u8"With",

        // Punctuators
        u8"Add",
        u8"InplaceAdd",
        u8"Sub",
        u8"InplaceSub",
        u8"Mul",
        u8"InplaceMul",
        u8"Div",
        u8"InplaceDiv",
        u8"Mod",
        u8"InplaceMod",
        u8"BitAnd",
        u8"InplaceBitAnd",
        u8"BitOr",
        u8"InplaceBitOr",
        u8"BitNot",
        u8"BitXor",
        u8"InplaceBitXor",
        u8"Less",
        u8"LessOrEqual",
        u8"LeftShift",
        u8"InplaceLeftShift",
        u8"Grater",
        u8"GraterOrEqual",
        u8"RightShift",
        u8"InplaceRightShift",
        u8"Equal",
        u8"NotEqual",
        u8"Assign",
        u8"Comma",
        u8"Colon",
        u8"Dot",
        u8"Ellipsis",
        u8"Arrow",
        u8"LeftParen",
        u8"RightParen",
        u8"LeftBrace",
        u8"RightBrace",
        u8"LeftSquare",
        u8"RightSquare",
    };

    return StringTable[static_cast<size_t>(type)];
}

std::ostream& operator <<(std::ostream& stream, const Token& token) {
    stream << "TokenType: " << TypeToString(token.Type) << ", Line: " << token.Position.Line << ", Column: " << token.Position.Column;
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const TokenData& data) {
    stream << "TokenType: " << TypeToString(data.Type) << ", Line: " << data.Line << ", Column: " << data.Column;
    return stream;
}

static inline bool Equals(const Token& token, const TokenData& expect, const SourceText& source) {
    bool equals = token.Type == expect.Type &&
                  token.Position.Line == expect.Line &&
                  token.Position.Column == expect.Column;

    if (token.Type == TokenType::Identifier ||
        token.Type == TokenType::Integer ||
        token.Type == TokenType::Float ||
        token.Type == TokenType::String) {
        equals = equals && token.Image.ToString() == expect.Image;
    }

    return equals;
}

#include <catch2/catch.hpp>

TEST_CASE("LexerTest", "[Lexer]") {
    SourceText source(TEST_SRC u8"/Scripts/LexerTest.script");
    ErrorReporter reporter;

    Lexer lexer(source, reporter);

    for (auto& expect : Expectations) {
        Token token = lexer.LexToken();

        INFO("Expected: " << expect << "(" << (expect.Image != nullptr ? expect.Image : "null") << "), Actual: " << token << "(" << (!token.Image.IsValid() ? "" : token.Image.ToString()) << ")");
        REQUIRE(Equals(token, expect, source));
    }

    REQUIRE_FALSE(reporter.HasError());
    REQUIRE_FALSE(reporter.HasWarning());
}