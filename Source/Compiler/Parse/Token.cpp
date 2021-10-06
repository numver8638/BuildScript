/*
 * Token.cpp
 * - Represent a lexeme in a script.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Token.h>

using namespace BuildScript;

// static
const char* Token::TypeToString(TokenType type) {
    static const char* const table[] = {
        "invalid",

        "EOF",
        "newline",
        "comment",

        "identifier",
        "integer",
        "float",
        "string",

        // Keywords
        "and",
        "as",
        "assert",
        "break",
        "case",
        "class",
        "continue",
        "const",
        "def",
        "default",
        "defined",
        "deinit",
        "dependsOn",
        "do",
        "doFirst",
        "doLast",
        "else",
        "except",
        "export",
        "extends",
        "false",
        "finally",
        "for",
        "from",
        "get",
        "if",
        "import",
        "in",
        "init",
        "inputs",
        "is",
        "match",
        "none",
        "not",
        "operator",
        "or",
        "outputs",
        "pass",
        "raise",
        "return",
        "self",
        "set",
        "static",
        "super",
        "task",
        "true",
        "try",
        "var",
        "while",
        "with",

        "+", "-", "*", "/", "%",
        "<<", ">>", "&", "|", "~", "^",
        "<", "<=", ">", ">=",
        "==", "!=",
        "=",
        "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=",
        ",", ":", ".", "...", "=>",
        "(", ")",
        "{", "}",
        "[", "]",
    };
    
    return table[static_cast<size_t>(type)];
}