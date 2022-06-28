/*
 * Statements.h
 * - Implementations of the IRStatement
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_STATEMENTS_H
#define BUILDSCRIPT_COMPILER_IR_STATEMENTS_H

#include <cassert>
#include <vector>

#include <BuildScript/Compiler/IR/IRStatement.h>
#include <BuildScript/Compiler/Utils/Value.h>

namespace BuildScript {
    class BasicBlock;   // Defined in <BuildScript/Compiler/IR/BasicBlock.h>
    class Symbol;       // Defined in <BuildScript/Compiler/Analysis/Symbol.h>

    class IRLoadConstOp final : public IRAssignStatement {
    private:
        Value m_value;

    public:
        IRLoadConstOp(SourcePosition pos, IRValue ret, Value value)
            : IRAssignStatement(IROpCode::LoadConst, pos, ret), m_value(std::move(value)) {}

        void Accept(IRVisitor*) override;

        const Value& GetValue() const { return m_value; }
    }; // end class IRLoadConstOp

    class IRDeclareSymbolOp final : public IRStatement {
    private:
        Symbol* m_symbol;
        IRValue m_value;

    public:
       IRDeclareSymbolOp(SourcePosition pos, Symbol* symbol, IRValue value)
           : IRStatement(IROpCode::DeclareSymbol, pos), m_symbol(symbol), m_value(value) {}

        void Accept(IRVisitor*) override;

        Symbol* GetSymbol() const { return m_symbol; }

        IRValue GetValue() const { return m_value; }

        void SetValue(IRValue value) { m_value = value; }
    }; // end class IRDeclareSymbolOp

    class IRLoadSymbolOp final : public IRAssignStatement {
    private:
        Symbol* m_symbol;

    public:
        IRLoadSymbolOp(SourcePosition pos, IRValue ret, Symbol* symbol)
            : IRAssignStatement(IROpCode::LoadSymbol, pos, ret), m_symbol(symbol) {}

        void Accept(IRVisitor*) override;

        Symbol* GetSymbol() const { return m_symbol; }
    }; // end class IRLoadSymbolOp

    class IRStoreSymbolOp final : public IRStatement {
    private:
        Symbol* m_symbol;
        IRValue m_value;

    public:
        IRStoreSymbolOp(SourcePosition pos, Symbol* symbol, IRValue value)
            : IRStatement(IROpCode::StoreSymbol, pos), m_symbol(symbol), m_value(value) {}

        void Accept(IRVisitor*) override;

        Symbol* GetSymbol() const { return m_symbol; }

        IRValue GetValue() const { return m_value; }

        void SetValue(IRValue value) { m_value = value; }
    }; // end class IRStoreSymbolOp

    class IRInplaceOp final : public IRAssignStatement {
    private:
        IRValue m_target, m_value;

    public:
        IRInplaceOp(IROpCode op, SourcePosition pos, IRValue ret, IRValue target, IRValue value)
            : IRAssignStatement(op, pos, ret), m_target(target), m_value(value) {
            assert((IROpCode::Add <= op) && (op <= IROpCode::Xor) && "not a inplace op.");
        }

        void Accept(IRVisitor*) override;

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }

        IRValue GetValue() const { return m_value; }

        void SetValue(IRValue value) { m_value = value; }
    }; // end class IRInplaceOp

    class IRBinaryOp final : public IRAssignStatement {
    private:
        IRValue m_left, m_right;

    public:
        IRBinaryOp(IROpCode op, SourcePosition pos, IRValue ret, IRValue left, IRValue right)
            : IRAssignStatement(op, pos, ret), m_left(left), m_right(right) {
            assert((IROpCode::Add <= op) && (op <= IROpCode::Xor) && "not a binary op.");
        }

        void Accept(IRVisitor*) override;

        IRValue GetLeft() const { return m_left; }

        void SetLeft(IRValue left) { m_left = left; }

        IRValue GetRight() const { return m_right; }

        void SetRight(IRValue right) { m_right = right; }
    }; // end class IRBinaryOp

    class IRUnaryOp final : public IRAssignStatement {
    private:
        IRValue m_value;

    public:
        IRUnaryOp(IROpCode op, SourcePosition pos, IRValue ret, IRValue value)
            : IRAssignStatement(op, pos, ret), m_value(value) {
            assert(((op == IROpCode::Not) || (op == IROpCode::Neg)) && "not a unary op.");
        }

        void Accept(IRVisitor*) override;

        IRValue GetValue() const { return m_value; }

        void SetValue(IRValue value) { m_value = value; }
    }; // end class IRUnaryOp

    class IRDefinedOp final : public IRAssignStatement {
    private:
        Value m_id;
        IRValue m_target;

    public:
        IRDefinedOp(SourcePosition pos, IRValue ret, Value id, IRValue target = IRInvalidValue)
            : IRAssignStatement(IROpCode::Defined, pos, ret), m_id(std::move(id)), m_target(target) {}

        void Accept(IRVisitor*) override;

        const Value& GetID() const { return m_id; }

        bool HasTarget() const { return m_target != IRInvalidValue; }

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }
    }; // end class IRDefinedOp

    class IRTestOp final : public IRAssignStatement {
    private:
        IRTestKind m_kind;
        IRValue m_left, m_right;

    public:
        IRTestOp(SourcePosition pos, IRValue ret, IRTestKind kind, IRValue left, IRValue right)
            : IRAssignStatement(IROpCode::Test, pos, ret), m_kind(kind), m_left(left), m_right(right) {}

        void Accept(IRVisitor*) override;

        IRTestKind GetKind() const { return m_kind; }

        IRValue GetLeft() const { return m_left; }

        void SetLeft(IRValue left) { m_left = left; }

        IRValue GetRight() const { return m_right; }

        void SetRight(IRValue right) { m_right = right; }
    }; // end class IRTestOp

    class IRCallOp final : public IRAssignStatement {
    private:
        IRValue m_target;
        std::vector<IRValue> m_args;

    public:
        IRCallOp(SourcePosition pos, IRValue ret, IRValue target, std::vector<IRValue>&& args)
            : IRAssignStatement(IROpCode::Call, pos, ret), m_target(target), m_args(std::move(args)) {}

        void Accept(IRVisitor*) override;

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }

        std::vector<IRValue>& GetArguments() { return m_args; }
    }; // end class IRCallOp

    class IRInvokeOp final : public IRAssignStatement {
    private:
        IRValue m_target;
        Value m_member;
        std::vector<IRValue> m_args;

    public:
        IRInvokeOp(SourcePosition pos, IRValue ret, IRValue target, Value member, std::vector<IRValue>&& args)
            : IRAssignStatement(IROpCode::Invoke, pos, ret), m_target(target), m_member(std::move(member)),
              m_args(std::move(args)) {}

        void Accept(IRVisitor*) override;

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }

        const Value& GetMember() const { return m_member; }

        std::vector<IRValue>& GetArguments() { return m_args; }
    }; // end class IRInvokeOp

    class IRGetMemberOp final : public IRAssignStatement {
    private:
        IRValue m_target;
        Value m_member;

    public:
        IRGetMemberOp(SourcePosition pos, IRValue ret, IRValue target, Value member)
            : IRAssignStatement(IROpCode::GetMember, pos, ret), m_target(target), m_member(std::move(member)) {}

        void Accept(IRVisitor*) override;

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }

        const Value& GetMember() const { return m_member; }
    }; // end class IRGetMemberOp

    class IRGetSubscriptOp final : public IRAssignStatement {
    private:
        IRValue m_target;
        IRValue m_index;

    public:
        IRGetSubscriptOp(SourcePosition pos, IRValue ret, IRValue target, IRValue index)
            : IRAssignStatement(IROpCode::GetSubscript, pos, ret), m_target(target), m_index(index) {}

        void Accept(IRVisitor*) override;

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }

        IRValue GetIndex() const { return m_index; }

        void SetIndex(IRValue index) { m_index = index; }
    }; // end class IRGetSubscriptOp

    class IRSetMemberOp final : public IRStatement {
    private:
        IRValue m_target;
        Value m_member;
        IRValue m_value;

    public:
        IRSetMemberOp(SourcePosition pos, IRValue target, Value member, IRValue value)
            : IRStatement(IROpCode::SetMember, pos), m_target(target), m_member(std::move(member)), m_value(value) {}

        void Accept(IRVisitor*) override;

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }

        const Value& GetMember() const { return m_member; }

        IRValue GetValue() const { return m_value; }

        void SetValue(IRValue value) { m_value = value; }
    }; // end class IRSetMemberOp

    class IRSetSubscriptOp final : public IRStatement {
    private:
        IRValue m_target, m_index, m_value;

    public:
        IRSetSubscriptOp(SourcePosition pos, IRValue target, IRValue index, IRValue value)
            : IRStatement(IROpCode::SetSubscript, pos), m_target(target), m_index(index), m_value(value) {}

        void Accept(IRVisitor*) override;

        IRValue GetTarget() const { return m_target; }

        void SetTarget(IRValue target) { m_target = target; }

        IRValue GetIndex() const { return m_index; }

        void SetIndex(IRValue index) { m_index = index; }

        IRValue GetValue() const { return m_value; }

        void SetValue(IRValue value) { m_value = value; }
    }; // end class IRSetSubscriptOp

    class IRBranchOp final : public IRStatement {
    private:
        BasicBlock* m_br;

    public:
        IRBranchOp(SourcePosition pos, BasicBlock* branch)
            : IRStatement(IROpCode::Br, pos), m_br(branch) {}

        void Accept(IRVisitor*) override;

        BasicBlock* GetBranch() const { return m_br; }

        void SetBranch(BasicBlock* block) { m_br = block; }
    }; // end class IRBranchOp

    class IRConditionalBranchOp final : public IRStatement {
    private:
        IRValue m_cond;
        BasicBlock* m_onTrue;
        BasicBlock* m_onFalse;

    public:
        IRConditionalBranchOp(SourcePosition pos, IRValue cond, BasicBlock* onTrue, BasicBlock* onFalse)
            : IRStatement(IROpCode::BrCond, pos), m_cond(cond), m_onTrue(onTrue), m_onFalse(onFalse) {}

        void Accept(IRVisitor*) override;

        IRValue GetCondition() const { return m_cond; }

        void SetCondition(IRValue cond) { m_cond = cond; }

        BasicBlock* GetBranchOnTrue() const { return m_onTrue; }

        void SetBranchOnTrue(BasicBlock* block) { m_onTrue = block; }

        BasicBlock* GetBranchOnFalse() const { return m_onFalse; }

        void SetBranchOnFalse(BasicBlock* block) { m_onFalse = block; }
    }; // end class IRConditionalBranchOp

    class IRJumpTableOp final : public IRStatement {
    public:
        using CaseTable = std::vector<std::tuple<Value, BasicBlock*>>;

    private:
        IRValue m_cond;
        BasicBlock* m_default;
        CaseTable m_cases;

    public:
        IRJumpTableOp(SourcePosition pos, IRValue cond, BasicBlock* _default, CaseTable&& cases)
            : IRStatement(IROpCode::JumpTable, pos), m_cond(cond), m_default(_default), m_cases(std::move(cases)) {}

        IRValue GetCondition() const { return m_cond; }

        void SetCondition(IRValue cond) { m_cond = cond; }

        BasicBlock* GetDefaultBlock() const { return m_default; }

        void SetDefaultBlock(BasicBlock* block) { m_default = block; }

        const CaseTable& GetCaseBlocks() const { return m_cases;}

        void UpdateCaseBlock(BasicBlock* old, BasicBlock* _new) {
            auto it = m_cases.begin();
            while (it != m_cases.end()) {
                auto& [value, block] = *it;
                if (block == old) {
                    it = m_cases.erase(it);
                    it = m_cases.insert(it, { value, _new });
                }
                else {
                    ++it;
                }
            }
        }

        void Accept(IRVisitor*) override;
    }; // end class IRJumpTableOp

    class IRReturnOp final : public IRStatement {
    private:
        IRValue m_retval;

    public:
        IRReturnOp(SourcePosition pos, IRValue retval)
            : IRStatement(IROpCode::Return, pos), m_retval(retval) {}

        void Accept(IRVisitor*) override;

        IRValue GetReturnValue() const { return m_retval; }

        void SetReturnValue(IRValue value) { m_retval = value; }
    }; // end class IRReturnOp

    class IRRaiseOp final : public IRStatement {
    private:
        IRValue m_throwable;

    public:
        IRRaiseOp(SourcePosition pos, IRValue throwable)
            : IRStatement(IROpCode::Raise, pos), m_throwable(throwable) {}

        void Accept(IRVisitor*) override;

        IRValue GetThrowable() const { return m_throwable; }

        void SetThrowable(IRValue value) { m_throwable = value; }
    }; // end class IRRaiseOp

    class IRAssertOp final : public IRStatement {
    private:
        IRValue m_cond, m_message;

    public:
        IRAssertOp(SourcePosition pos, IRValue cond, IRValue message = IRInvalidValue)
            : IRStatement(IROpCode::Assert, pos), m_cond(cond), m_message(message) {}

        IRValue GetCondition() const { return m_cond; }

        void SetCondition(IRValue cond) { m_cond = cond; }

        IRValue GetMessage() const { return m_message; }

        void SetMessage(IRValue message) { m_message = message; }

        void Accept(IRVisitor*) override;
    }; // end class IRAssertOp

    class IRSelectOp final : public IRAssignStatement {
    private:
        std::vector<IRValue> m_targets;

    public:
        IRSelectOp(IRValue ret, std::vector<IRValue>&& targets)
            : IRAssignStatement(IROpCode::Select, SourcePosition::Empty(), ret), m_targets(std::move(targets)) {}

        void Accept(IRVisitor*) override;

        std::vector<IRValue>& GetTargets() { return m_targets; }

        void AddTarget(IRValue target) { m_targets.push_back(target); }
    }; // end class IRSelectOp

    class IRMakeOp final : public IRAssignStatement {
    private:
        std::vector<IRValue> m_args;

    public:
        IRMakeOp(IROpCode op, SourcePosition pos, IRValue ret, std::vector<IRValue>&& args)
            : IRAssignStatement(op, pos, ret), m_args(std::move(args)) {
            assert(((IROpCode::MakeList <= op) && (op <= IROpCode::MakeClosure)) && "not a make op.");
        }

        void Accept(IRVisitor*) override;

        std::vector<IRValue>& GetArguments() { return m_args; }
    }; // end class IRMakeOp

    class IRImportOp final : public IRStatement {
    private:
        IRValue m_path;

    public:
        IRImportOp(SourcePosition pos, IRValue path)
            : IRStatement(IROpCode::Import, pos), m_path(path) {}

        void Accept(IRVisitor*) override;

        IRValue GetPath() const { return m_path; }

        void SetPath(IRValue path) { m_path = path; }
    }; // end class IRImportOp

    class IRExportOp final : public IRStatement {
    private:
        Symbol* m_symbol;

    public:
        IRExportOp(SourcePosition pos, Symbol* symbol)
            : IRStatement(IROpCode::Export, pos), m_symbol(symbol) {}

        void Accept(IRVisitor*) override;

        Symbol* GetSymbol() const { return m_symbol; }
    }; // end class IRExportOp
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_IR_STATEMENTS_H