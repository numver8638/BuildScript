/*
 * Opcode.h
 * - Represents instruction of the interpreter.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_BYTECODE_OPCODE_H
#define BUILDSCRIPT_BYTECODE_OPCODE_H

#include <cstdint>
#include <string_view>

namespace BuildScript {
    /**
     * @brief Represents instruction of the interpreter.
     */
    enum class OpCode : uint8_t {
        NOP,
        Break,

        Move,
        LoadGlobal,
        LoadConst,
        LoadInt,
        LoadTrue,
        LoadFalse,
        LoadNone,
        LoadArg,
        LoadReturn,
        LoadExc,
        StoreGlobal,
        PushArg,

        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Shr,
        Shl,
        And,
        Or,
        Xor,
        Not,
        Neg,

        InplaceAdd,
        InplaceSub,
        InplaceMul,
        InplaceDiv,
        InplaceMod,
        InplaceShr,
        InplaceShl,
        InplaceAnd,
        InplaceOr,
        InplaceXor,

        Test,
        DefinedGlobal,
        DefinedProperty,

        GetMember,
        GetSubscript,
        GetField,
        GetSuper,
        GetBound,
        SetMember,
        SetSubscript,
        SetField,

        Br,
        BrTrue,
        BrFalse,
        JumpTable,
        Call,
        Invoke,
        Return,
        ReturnNone,
        Raise,

        Assert,
        Import,
        Export,
        MakeList,
        MakeMap,
        MakeClosure,
        DeclareGlobal,
        DeclareFunction,
        DeclareClass,
        DeclareField,

        Max = DeclareField
    }; // end enum OpCode

    /**
     * @brief Sub-opcode of TEST opcode.
     */
    enum class TestOpCode : uint8_t {
        Grater,
        GraterEqual,
        Less,
        LessEqual,
        Equal,
        NotEqual,
        EqualType,
        NotEqualType,
        Contain,
        NotContain,

        Max = NotContain
    }; // end enum TestOpCode

    /**
     * @brief Convert @c OpCode to readable string.
     * @param op an @c OpCode to convert.
     * @return a @c std::string_view representing the opcode.
     */
    std::string_view OpCode2String(OpCode op);

    /**
     * @brief Convert @c TestOpCode to readable string.
     * @param op a @c TestOpCode to convert.
     * @return a @c std::string_view representing the test opcode.
     */
    std::string_view TestOpCode2String(TestOpCode op);
} // end namespace BuildScript

#endif // BUILDSCRIPT_BYTECODE_OPCODE_H