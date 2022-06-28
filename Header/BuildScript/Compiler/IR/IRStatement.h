/*
 * IRStatement.h
 * - Represents an IR instruction.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_IRSTATEMENT_H
#define BUILDSCRIPT_COMPILER_IR_IRSTATEMENT_H

#include <cstdint>
#include <limits>
#include <string_view>

#include <BuildScript/Compiler/ManagedObject.h>
#include <BuildScript/Compiler/SourcePosition.h>

namespace BuildScript {
    class IRVisitor; // Defined in <BuildScript/Compiler/IR/IRDumper.h>

    /**
     * @brief Represents readonly single assigned value.
     */
    enum class IRValue : uint32_t {};

    /**
     * @brief Represents invalid @c IRValue.
     */
    constexpr IRValue IRInvalidValue = static_cast<IRValue>(std::numeric_limits<uint32_t>::max());

    /**
     * @brief Represents a opcode of the IR.
     */
    enum class IROpCode {
        LoadConst,
        LoadSymbol,
        StoreSymbol,
        DeclareSymbol,
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
        Test,
        Defined,
        GetMember,
        GetSubscript,
        SetMember,
        SetSubscript,
        Br,
        BrCond,
        JumpTable,
        Call,
        Invoke,
        Raise,
        Return,
        Assert,
        Select,
        MakeList,
        MakeMap,
        MakeClosure,
        Import,
        Export
    }; // end enum IROpCode

    std::string_view IROpCodeToString(IROpCode op);

    /**
     * @brief Represents kind of the test.
     */
    enum class IRTestKind {
        Grater,
        GraterOrEqual,
        Less,
        LessOrEqual,
        Equal,
        NotEqual,
        Instance,
        NotInstance,
        Contain,
        NotContain
    }; // end class IRTestKind

    /**
     * @brief Represents an IR instruction.
     */
    class IRStatement : public ManagedObject {
    private:
        IROpCode m_op;
        SourcePosition m_pos;

    protected:
        IRStatement(IROpCode op, SourcePosition pos)
            : m_op(op), m_pos(pos) {}

    public:
        /**
         * @brief Get an opcode of the statement.
         * @return an @c IROpCode representing a opcode of the statement.
         */
        IROpCode GetOp() const { return m_op; }

        /**
         * @brief Get the position of the statement representing in the source.
         * @return a @c SourcePosition where the statement positioned.
         */
        SourcePosition GetPosition() const { return m_pos; }

        /**
         * @brief Test the statement represents end of flow in the basic block.
         * @return @c true if the statement is terminal, otherwise @c false.
         */
        bool IsTerminal() const {
            switch (m_op) {
                case IROpCode::Br:
                case IROpCode::BrCond:
                case IROpCode::JumpTable:
                case IROpCode::Return:
                case IROpCode::Raise:
                    return true;

                default:
                    return false;
            }
        }

        virtual void Accept(IRVisitor*) = 0;
    }; // end class IRStatement

    /**
     * @brief Represents an IR instruction with return value.
     */
    class IRAssignStatement : public IRStatement {
    private:
        IRValue m_ret;

    protected:
        IRAssignStatement(IROpCode op, SourcePosition pos, IRValue ret)
            : IRStatement(op, pos), m_ret(ret) {}

    public:
        /**
         * @brief Get return value of the statement.
         * @return an @c IRValue representing return value of the statement.
         */
        IRValue GetReturnValue() const { return m_ret; }
    }; // end class IRAssignStatement
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_IR_IRSTATEMENT_H