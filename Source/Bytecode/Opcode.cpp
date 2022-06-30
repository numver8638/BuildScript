/*
 * OpCode.cpp
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Bytecode/Opcode.h>

std::string_view BuildScript::OpCode2String(OpCode op) {
    static std::string_view table[] = {
        "NOP",
        "BREAK",
        "MOV",
        "LOAD_GLOBAL",
        "LOAD_CONST",
        "LOAD_INT",
        "LOAD_TRUE",
        "LOAD_FALSE",
        "LOAD_NONE",
        "LOAD_ARG",
        "LOAD_RETURN",
        "LOAD_EXC",
        "STORE_GLOBAL",
        "PUSH_ARG",
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
        "INPLACE_ADD",
        "INPLACE_SUB",
        "INPLACE_MUL",
        "INPLACE_DIV",
        "INPLACE_MOD",
        "INPLACE_SHR",
        "INPLACE_SHL",
        "INPLACE_AND",
        "INPLACE_OR",
        "INPLACE_XOR",
        "TEST",
        "DEFINED_GLOBAL",
        "DEFINED_PROPERTY",
        "GET_MEMBER",
        "GET_SUBSCRIPT",
        "GET_FIELD",
        "GET_SUPER",
        "GET_BOUND",
        "SET_MEMBER",
        "SET_SUBSCRIPT",
        "SET_FIELD",
        "BR",
        "BR_TRUE",
        "BR_FALSE",
        "JUMP_TABLE",
        "CALL",
        "INVOKE",
        "RETURN",
        "RETURN_NONE",
        "RAISE",
        "ASSERT",
        "IMPORT",
        "EXPORT",
        "MAKE_LIST",
        "MAKE_MAP",
        "MAKE_CLOSURE",
        "DECLARE_GLOBAL",
        "DECLARE_FUNCTION",
        "DECLARE_CLASS",
        "DECLARE_FIELD",
    };

    return (op <= OpCode::Max) ? table[static_cast<size_t>(op)] : "INVALID";
}

std::string_view BuildScript::TestOpCode2String(TestOpCode op) {
    static const std::string_view table[] = {
        "GRATER",
        "GRATER_EQUAL",
        "LESS",
        "LESS_EQUAL",
        "EQUAL",
        "NOT_EQUAL",
        "EQUAL_TYPE",
        "NOT_EQUAL_TYPE",
        "CONTAIN",
        "NOT_CONTAIN"
    };

    return (op <= TestOpCode::Max) ? table[static_cast<size_t>(op)] : "INVALID";
}