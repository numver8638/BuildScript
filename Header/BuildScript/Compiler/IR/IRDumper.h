/*
 * IRDumper.h
 * - Print IR code.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_IR_IRDUMPER_H
#define BUILDSCRIPT_IR_IRDUMPER_H

#include <iostream>
#include <vector>

#include <BuildScript/Compiler/IR/IRCodeBlock.h>
#include <BuildScript/Compiler/IR/IRVisitor.h>

namespace BuildScript {
    class BasicBlock;

    /**
     * @brief Print IR code.
     */
    class IRDumper final : public IRVisitor {
    private:
        std::ostream& m_out;

        void Visit(IRLoadConstOp&) override;
        void Visit(IRLoadSymbolOp&) override;
        void Visit(IRStoreSymbolOp&) override;
        void Visit(IRDeclareSymbolOp&) override;
        void Visit(IRBinaryOp&) override;
        void Visit(IRInplaceOp &) override;
        void Visit(IRUnaryOp&) override;
        void Visit(IRDefinedOp&) override;
        void Visit(IRTestOp&) override;
        void Visit(IRCallOp&) override;
        void Visit(IRInvokeOp&) override;
        void Visit(IRGetMemberOp&) override;
        void Visit(IRGetSubscriptOp&) override;
        void Visit(IRSetMemberOp&) override;
        void Visit(IRSetSubscriptOp&) override;
        void Visit(IRBranchOp&) override;
        void Visit(IRConditionalBranchOp&) override;
        void Visit(IRJumpTableOp&) override;
        void Visit(IRReturnOp&) override;
        void Visit(IRRaiseOp&) override;
        void Visit(IRAssertOp&) override;
        void Visit(IRSelectOp&) override;
        void Visit(IRMakeOp&) override;
        void Visit(IRImportOp &) override;
        void Visit(IRExportOp &) override;

        void Dump(BasicBlock& block);

    public:
        explicit IRDumper(std::ostream& out = std::cout)
            : m_out(out) {}

        void Dump(std::vector<IRCodeBlock>& codes);
    }; // end class IRDumper
} // end namespace BuildScript

#endif // BUILDSCRIPT_IR_IRDUMPER_H