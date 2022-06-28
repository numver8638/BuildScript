/*
 * BasicBlock.h
 * - Represents a block of IR statements.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_BASICBLOCK_H
#define BUILDSCRIPT_COMPILER_IR_BASICBLOCK_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <BuildScript/Compiler/IR/IRStatement.h>
#include <BuildScript/Compiler/Utils/Value.h>
#include <BuildScript/Compiler/ManagedObject.h>

namespace BuildScript {
    class Symbol;

    /**
     * @brief Represents a block of IR statements.
     */
    class BasicBlock final : public ManagedObject, public std::vector<IRStatement*> {
        using super = std::vector<IRStatement*>;

    private:
        std::string m_label;
        std::unordered_set<BasicBlock*> m_preds, m_succs;

    public:
        /**
         * @brief
         * @param label
         */
        explicit BasicBlock(std::string label)
            : m_label(std::move(label)) {}

        std::unordered_map<Symbol*, IRValue> Defined;
        std::unordered_map<Value, IRValue> Consts;

        /**
         * @brief
         * @return
         */
        std::string_view GetLabel() const { return m_label; }

        void AddSuccessor(BasicBlock* block) {
            m_succs.emplace(block);
            block->m_preds.emplace(this);
        }

        void AddSuccessorOnly(BasicBlock* block) {
            m_succs.emplace(block);
        }

        void AddSuccessors(std::initializer_list<BasicBlock*> list) {
            for (auto* block : list) {
                AddSuccessor(block);
            }
        }

        const std::unordered_set<BasicBlock*>& GetSuccessors() const { return m_succs; }

        const std::unordered_set<BasicBlock*>& GetPredecessors() const { return m_preds; }

        void ClearLinkages() {
            m_preds.clear();
            m_succs.clear();
        }
    }; // end class BasicBlock
}

#endif // BUILDSCRIPT_COMPILER_IR_BASICBLOCK_H