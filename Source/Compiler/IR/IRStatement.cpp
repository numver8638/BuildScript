/*
 * IRStatement.cpp
 * - Represents an IR instruction.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRStatement.h>

namespace BuildScript {
    std::string_view IROpCodeToString(IROpCode op) {
        static std::string_view Table[] = {
            "LOAD_CONST",
            "LOAD_SYMBOL",
            "STORE_SYMBOL",
            "DECLARE_SYMBOL",
            "ADD",
            "SUB",
            "MUL",
            "DIV",
            "MOD",
            "SHR",
            "SHL",
            "AND",
            "OR",
            "XOR",
            "NOT",
            "NEG",
            "TEST",
            "DEFINED",
            "GET_MEMBER",
            "GET_SUBSCRIPT",
            "SET_MEMBER",
            "SET_SUBSCRIPT",
            "BR",
            "BR_COND",
            "JUMP_TABLE",
            "CALL",
            "INVOKE",
            "RAISE",
            "RETURN",
            "ASSERT",
            "SELECT",
            "MAKE_LIST",
            "MAKE_MAP",
            "MAKE_CLOSURE",
            "IMPORT",
            "EXPORT"
        };

        return Table[static_cast<size_t>(op)];
    }
} // end namespace BuildScript