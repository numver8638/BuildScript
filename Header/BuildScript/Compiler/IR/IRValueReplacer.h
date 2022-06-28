/*
 * IRValueReplacer.h
 * - Replaces IRValue in IRStatement.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_IRVALUEREPLACER_H
#define BUILDSCRIPT_COMPILER_IR_IRVALUEREPLACER_H

#include <BuildScript/Compiler/IR/IRStatement.h>
#include <BuildScript/Compiler/IR/IRVisitor.h>

namespace BuildScript {
    /**
     * @brief Replaces @c IRValue in @c IRStatement.
     */
    class IRValueReplacer final : IRVisitor {
    private:
        const IRValue m_from, m_to;

        void Visit(IRLoadConstOp& op) override;

        void Visit(IRLoadSymbolOp& op) override;

        void Visit(IRStoreSymbolOp& op) override;

        void Visit(IRDeclareSymbolOp& op) override;

        void Visit(IRBinaryOp& op) override;

        void Visit(IRInplaceOp& op) override;

        void Visit(IRUnaryOp& op) override;

        void Visit(IRDefinedOp& op) override;

        void Visit(IRTestOp& op) override;

        void Visit(IRCallOp& op) override;

        void Visit(IRInvokeOp& op) override;

        void Visit(IRGetMemberOp& op) override;

        void Visit(IRGetSubscriptOp& op) override;

        void Visit(IRSetMemberOp& op) override;

        void Visit(IRSetSubscriptOp& op) override;

        void Visit(IRBranchOp& op) override;

        void Visit(IRConditionalBranchOp& op) override;

        void Visit(IRJumpTableOp& op) override;

        void Visit(IRReturnOp& op) override;

        void Visit(IRRaiseOp& op) override;

        void Visit(IRAssertOp& op) override;

        void Visit(IRSelectOp& op) override;

        void Visit(IRMakeOp& op) override;

        void Visit(IRImportOp& op) override;

        void Visit(IRExportOp& op) override;

    public:
        IRValueReplacer(IRValue from, IRValue to)
            : m_from(from), m_to(to) {}

        void Replace(BasicBlock* block);
    }; // end class IRValueReplacer
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_IR_IRVALUEREPLACER_H