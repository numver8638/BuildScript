/*
 * OperatorKind.h
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_OPERATORKIND_H
#define BUILDSCRIPT_COMPILER_AST_OPERATORKIND_H

namespace BuildScript {
    /**
     * @brief .
     */
    enum class OperatorKind {
        Invalid,
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        LeftShift,
        RightShift,
        BitAnd,
        BitOr,
        BitNot,
        BitXor,
        InplaceAdd,
        InplaceSub,
        InplaceMul,
        InplaceDiv,
        InplaceMod,
        InplaceLeftShift,
        InplaceRightShift,
        InplaceBitAnd,
        InplaceBitOr,
        InplaceBitXor,
        Index
    }; // end class OperatorKind

    const char* OperatorKindToString(OperatorKind kind);
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_OPERATORKIND_H