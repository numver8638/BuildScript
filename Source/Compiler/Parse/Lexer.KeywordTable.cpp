/*
 * Lexer.KeywordTable.cpp
 * - Token scanner for BuildScript language.
 *   This file has perfect hash table for finding keywords.
 *   the table is constructed from modified gpref-generated source.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Lexer.h>

using namespace BuildScript;

#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 9
#define MAX_HASH_VALUE 108

static size_t GetIndex(const std::string& str) {
    static const unsigned char asso_values[] = {
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109,   0,   0,   0,
         15,   0,  35,  25,  10,  10, 109,   0,   5,   0,
          0,  10,  45, 109,  45,   5,  15,   0,  20,  10,
         30, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109, 109, 109, 109, 109,
        109, 109, 109, 109, 109, 109
    };

    auto hval = str.length();

    switch (hval) {
        default:
            hval += asso_values[(unsigned char)str[3]];
            [[fallthrough]];

        case 3:
        case 2:
            hval += asso_values[(unsigned char)str[1]];
            [[fallthrough]];

        case 1:
            hval += asso_values[(unsigned char)str[0]];
            break;
    }

    return hval;
}

// static
TokenType Lexer::GetKeyword(const std::string& str) {
    static const struct {
        const char* Name;
        TokenType Type;
    } wordlist[] = {
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "and",       TokenType::And },
        { "case",      TokenType::Case },
        { "match",     TokenType::Match },
        { "",          TokenType::Identifier },
        { "as",        TokenType::As },
        { "set",       TokenType::Set },
        { "else",      TokenType::Else },
        { "super",     TokenType::Super },
        { "assert",    TokenType::Assert },
        { "in",        TokenType::In },
        { "not",       TokenType::Not },
        { "none",      TokenType::None },
        { "class",     TokenType::Class },
        { "inputs",    TokenType::Inputs },
        { "is",        TokenType::Is },
        { "def",       TokenType::Def },
        { "task",      TokenType::Task },
        { "const",     TokenType::Const },
        { "deinit",    TokenType::Deinit },
        { "default",   TokenType::Default },
        { "var",       TokenType::Var },
        { "dependsOn", TokenType::DependsOn },
        { "",          TokenType::Identifier },
        { "import",    TokenType::Import },
        { "do",        TokenType::Do },
        { "get",       TokenType::Get },
        { "init",      TokenType::Init },
        { "while",     TokenType::While },
        { "doLast",    TokenType::DoLast },
        { "defined",   TokenType::Defined },
        { "continue",  TokenType::Continue },
        { "with",      TokenType::With },
        { "",          TokenType::Identifier },
        { "except",    TokenType::Except },
        { "extends",   TokenType::Extends },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "static",    TokenType::Static },
        { "doFirst",   TokenType::DoFirst },
        { "",          TokenType::Identifier },
        { "self",      TokenType::Self },
        { "false",     TokenType::False },
        { "export",    TokenType::Export },
        { "if",        TokenType::If },
        { "for",       TokenType::For },
        { "",          TokenType::Identifier },
        { "break",     TokenType::Break },
        { "return",    TokenType::Return },
        { "finally",   TokenType::Finally },
        { "",          TokenType::Identifier },
        { "pass",      TokenType::Pass },
        { "raise",     TokenType::Raise },
        { "",          TokenType::Identifier },
        { "or",        TokenType::Or },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "outputs",   TokenType::Outputs },
        { "try",       TokenType::Try },
        { "true",      TokenType::True },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "from",      TokenType::From },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "",          TokenType::Identifier },
        { "operator",  TokenType::Operator }
    };

    if (str.length() <= MAX_WORD_LENGTH && str.length() >= MIN_WORD_LENGTH) {
        auto key = GetIndex(str);

        if (key <= MAX_HASH_VALUE) {
            auto& word = wordlist[key];

            if (str == word.Name) {
                return word.Type;
            }
        }
    }

    return TokenType::Identifier;
}