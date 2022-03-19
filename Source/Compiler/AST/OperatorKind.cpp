/*
 * OperatorKind.cpp
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/OperatorKind.h>

#include <cstddef>

namespace BuildScript {
    const char* OperatorKindToString(OperatorKind kind) {
        static const char* table[] = {
            "<invalid>",
            "+",
            "-",
            "*",
            "/",
            "%",
            "<<",
            ">>",
            "&",
            "|",
            "~",
            "^",
            "+=",
            "-=",
            "*=",
            "/=",
            "%=",
            "<<=",
            ">>=",
            "&=",
            "|=",
            "^=",
        };

        return table[static_cast<size_t>(kind) - static_cast<size_t>(OperatorKind::Invalid)];
    }
} // end namespace BuildScript