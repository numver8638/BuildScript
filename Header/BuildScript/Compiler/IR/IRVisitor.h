/*
 * IRVisitor.h
 * - Visitor class for IRStatement.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_IRVISITOR
#define BUILDSCRIPT_COMPILER_IR_IRVISITOR

#include <BuildScript/Compiler/IR/Statements.h>

namespace BuildScript {
    /**
     * @brief Visitor class for @c IRStatement.
     */
    class IRVisitor {
    public:
        virtual ~IRVisitor() = default;

        virtual void Visit(IRLoadConstOp&) = 0;
        virtual void Visit(IRLoadSymbolOp&) = 0;
        virtual void Visit(IRStoreSymbolOp&) = 0;
        virtual void Visit(IRDeclareSymbolOp&) = 0;
        virtual void Visit(IRBinaryOp&) = 0;
        virtual void Visit(IRInplaceOp&) = 0;
        virtual void Visit(IRUnaryOp&) = 0;
        virtual void Visit(IRDefinedOp&) = 0;
        virtual void Visit(IRTestOp&) = 0;
        virtual void Visit(IRCallOp&) = 0;
        virtual void Visit(IRInvokeOp&) = 0;
        virtual void Visit(IRGetMemberOp&) = 0;
        virtual void Visit(IRGetSubscriptOp&) = 0;
        virtual void Visit(IRSetMemberOp&) = 0;
        virtual void Visit(IRSetSubscriptOp&) = 0;
        virtual void Visit(IRBranchOp&) = 0;
        virtual void Visit(IRConditionalBranchOp&) = 0;
        virtual void Visit(IRJumpTableOp&) = 0;
        virtual void Visit(IRReturnOp&) = 0;
        virtual void Visit(IRRaiseOp&) = 0;
        virtual void Visit(IRAssertOp&) = 0;
        virtual void Visit(IRSelectOp&) = 0;
        virtual void Visit(IRMakeOp&) = 0;
        virtual void Visit(IRImportOp&) = 0;
        virtual void Visit(IRExportOp&) = 0;
    }; // end class IRVisitor
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_IR_IRVISITOR