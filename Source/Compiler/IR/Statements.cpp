/*
 * Statements.cpp
 * - Implementations of the IRStatement
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/Statements.h>

#include <BuildScript/Compiler/IR/IRVisitor.h>

using namespace BuildScript;

void IRLoadConstOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRLoadSymbolOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRStoreSymbolOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRDeclareSymbolOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRBinaryOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRInplaceOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRUnaryOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRDefinedOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRTestOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRCallOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRInvokeOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRGetMemberOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRGetSubscriptOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRSetMemberOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRSetSubscriptOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRBranchOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRConditionalBranchOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRJumpTableOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRReturnOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRRaiseOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRAssertOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRSelectOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRMakeOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRImportOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}

void IRExportOp::Accept(IRVisitor* visitor) {
    visitor->Visit(*this);
}