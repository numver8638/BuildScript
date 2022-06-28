/*
 * IRBuilder.cpp
 * - Builder class for IR codes.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRBuilder.h>

#include <algorithm>
#include <cassert>
#include <unordered_set>

#include <BuildScript/Compiler/Symbol/Symbol.h>
#include <BuildScript/Compiler/IR/Statements.h>
#include <BuildScript/Compiler/IR/IRValueReplacer.h>

using namespace BuildScript;

static void Walk(BasicBlock* block, std::vector<BasicBlock*>& ordered, std::unordered_set<BasicBlock*>& visited) {
    visited.insert(block);

    for (auto* succ : block->GetSuccessors()) {
        auto it = visited.find(succ);

        if (it == visited.end()) {
            Walk(succ, ordered, visited);
        }
    }

    ordered.push_back(block);
}

static std::vector<BasicBlock*> ReorderBB(std::vector<BasicBlock*> blocks) {
    std::vector<BasicBlock*> ordered;
    std::unordered_set<BasicBlock*> visited;

    Walk(blocks.front(), ordered, visited);

    std::reverse(ordered.begin(), ordered.end());
    return std::move(ordered);
}

IRCodeBlock IRBuilder::Finalize() {
    auto IsBranchOp = [](IRStatement* ir) { return ir->IsTerminal(); };

    // Dead code elimination
    for (auto* block : m_blocks) {
        auto it = std::find_if(block->begin(), block->end(), IsBranchOp);

        if (it != block->end()) {
            // remove IRs after branch
            block->erase(++it, block->end());

            // sanity check
            assert(block->back() != nullptr && IsBranchOp(block->back()));
        }
    }

    // sort by execution order
    m_blocks = ReorderBB(std::move(m_blocks));

    // insert select(phi) statements
    for (auto& [block, ops] : m_candidates) {
        std::vector<IRStatement*> op;

        std::transform(ops.begin(), ops.end(), std::back_inserter(op), [](auto& args) {
            return std::get<1>(args);
        });

        block->insert(block->begin(), op.begin(), op.end());
    }

    return { m_name, std::move(m_blocks), m_vararg, std::move(m_handlers), std::move(m_args)};
}

IRValue FindConst(BasicBlock* block, const Value& value) {
    auto it = block->Consts.find(value);
    if (it != block->Consts.end()) {
        return it->second;
    }

    IRValue ret = IRInvalidValue;

    for (auto* pred : block->GetPredecessors()) {
        auto val = FindConst(pred, value);

        if (val == IRInvalidValue) { continue; }

        if (ret == IRInvalidValue) { ret = val; }

        if (ret != val) { return IRInvalidValue; }
    }

    return ret;
}

IRValue IRBuilder::ReadConst(SourcePosition pos, Value value) {
    auto reg = FindConst(m_block, value);

    if (reg == IRInvalidValue) {
        reg = GetNewRegister();
        InsertBack<IRLoadConstOp>(pos, reg, value);

        GetCurrentBlock().Consts.emplace(std::move(value), reg);
    }

    return reg;
}

inline bool IsGlobal(Symbol* symbol) {
    auto* var = symbol->As<VariableSymbol>();

    if (var == nullptr) { return true; }

    switch (var->GetVariableType()) {
        case VariableType::Local:
        case VariableType::Parameter:
        case VariableType::Implicit:
        case VariableType::Except:
            return false;

        default:
            return true;
    }
}

// Referenced from the paper 'Simple and Efficient Construction of Static Single Assignment Form'.
IRValue IRBuilder::ReadSymbol(Symbol* symbol, BasicBlock* block) {
    auto it = block->Defined.find(symbol);

    if (it != block->Defined.end()) {
        return it->second;
    }

    IRValue reg;
    if (m_sealed.find(block) == m_sealed.end()) {
        reg = GetNewRegister();
        auto* select = new (m_context.GetAllocator()) IRSelectOp(reg, {});

        m_incompletePhis[block][symbol] = select;
    }
    else if (block->GetPredecessors().size() == 1) {
        reg = ReadSymbol(symbol, *(block->GetPredecessors().begin()));
    }
    else {
        auto* select = new (m_context.GetAllocator()) IRSelectOp(GetNewRegister(), {});
        block->Defined.insert_or_assign(symbol, select->GetReturnValue());

        reg = AddPhiOps(select, symbol, block);

        if (reg == select->GetReturnValue()) {
            m_candidates[block][symbol] = select;
        }
    }

    block->Defined.insert_or_assign(symbol, reg);
    return reg;
}

IRValue IRBuilder::AddPhiOps(IRSelectOp* select, Symbol* symbol, BasicBlock* block) {
    std::unordered_set<IRValue> values;

    for (auto* pred : block->GetPredecessors()) {
        values.insert(ReadSymbol(symbol, pred));
    }

    std::for_each(values.begin(), values.end(), [&select](auto val) { select->AddTarget(val); });

    return OptPhi(select, symbol, block);
}

IRValue IRBuilder::OptPhi(IRSelectOp* select, Symbol* symbol, BasicBlock* block) {
    auto same = IRInvalidValue;
    auto phi = select->GetReturnValue();

    for (auto op : select->GetTargets()) {
        if (op == same || op == phi) { continue; }

        if (same != IRInvalidValue) { return phi; }

        same = op;
    }

    if (same == IRInvalidValue) {
        same = GetNewRegister();
        block->insert(block->begin(), new (m_context.GetAllocator()) IRLoadSymbolOp(SourcePosition(), same, symbol));
    }

    for (auto* b : m_blocks) {
         IRValueReplacer(phi, same).Replace(b);
    }

    m_candidates[block].erase(symbol);

    for (auto& [b, symbols] : m_candidates) {
        auto symbols_it = symbols.find(symbol);
        if (symbols_it == symbols.end()) { continue; }

        auto& [_, S] = *symbols_it;

        auto begin = S->GetTargets().begin();
        auto end = S->GetTargets().end();
        auto user = false;

        auto it = std::remove(begin, end, phi);
        while (it != end) {
            user = true;
            S->GetTargets().erase(it);
            it = std::remove(it, end, phi);
        }

        if (!user) { continue; }

        OptPhi(S, symbol, b);
    }

    return same;
}

void IRBuilder::SealBlock(BasicBlock* block) {
    for (auto& [symbol, select] : m_incompletePhis[block]) {
        auto res = AddPhiOps(select, symbol, block);

         if (res == select->GetReturnValue()) {
            m_candidates[block][symbol] = select;
         }
    }

    m_incompletePhis.erase(block);
    m_sealed.emplace(block);
}

IRValue IRBuilder::ReadSymbol(SourcePosition pos, Symbol* symbol) {
    auto it = GetCurrentBlock().Defined.find(symbol);

    if (it != GetCurrentBlock().Defined.end()) {
        return it->second;
    }
    else if (IsGlobal(symbol)) {
        auto reg = GetNewRegister();
        InsertBack<IRLoadSymbolOp>(pos, reg, symbol);
        GetCurrentBlock().Defined.insert_or_assign(symbol, reg);

        return reg;
    }
    else {
        return ReadSymbol(symbol, &(GetCurrentBlock()));
    }
}

void IRBuilder::WriteSymbol(SourcePosition pos, Symbol* symbol, IRValue value) {
    GetCurrentBlock().Defined.insert_or_assign(symbol, value);

    if (IsGlobal(symbol)) {
        InsertBack<IRStoreSymbolOp>(pos, symbol, value);
    }
}