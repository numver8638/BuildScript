/*
 * IRBuilder.h
 * - Builder class for IR codes.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_IRBUILDER_H
#define BUILDSCRIPT_COMPILER_IR_IRBUILDER_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <fmt/format.h>

#include <BuildScript/Compiler/Context.h>
#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Compiler/IR/BasicBlock.h>
#include <BuildScript/Compiler/IR/IRCodeBlock.h>
#include <BuildScript/Compiler/IR/IRStatement.h>
#include <BuildScript/Compiler/Utils/Value.h>

namespace BuildScript {
    class Context;
    class IRSelectOp;

    /**
     * @brief Builder class for IR codes.
     */
    class IRBuilder final {
    private:
        Context& m_context;
        const std::string m_name;
        std::vector<Symbol*> m_args;
        const bool m_vararg;

        uint32_t m_index = 0;
        uint32_t m_blockCount = 0;

        BasicBlock* m_entry;
        BasicBlock* m_block = nullptr;

        std::vector<BasicBlock*> m_blocks;

        std::vector<ExceptInfo> m_handlers;
        std::unordered_map<BasicBlock*, std::unordered_map<Symbol*, IRSelectOp*>> m_candidates;
        std::unordered_map<BasicBlock*, std::unordered_map<Symbol*, IRSelectOp*>> m_incompletePhis;
        std::unordered_set<BasicBlock*> m_sealed;

        IRValue ReadSymbol(Symbol*, BasicBlock*);
        IRValue AddPhiOps(IRSelectOp*, Symbol*, BasicBlock*);
        IRValue OptPhi(IRSelectOp*, Symbol*, BasicBlock*);

    public:
        explicit IRBuilder(Context& context, std::string name, std::vector<Symbol*> args, bool vararg)
            : m_context(context), m_name(std::move(name)), m_args(std::move(args)), m_vararg(vararg) {
            m_entry = m_block = new (m_context.GetAllocator()) BasicBlock("L0");
            m_blocks.push_back(m_entry);
            m_sealed.emplace(m_entry);
        }

        /**
         * @brief Finalize the builder and create @c IRCodeBlock.
         * @return an @c IRCodeBlock created by the builder.
         */
        IRCodeBlock Finalize();

        /**
         * @brief Create new block.
         * @return a @c BasicBlock freshly created.
         */
        BasicBlock* CreateNewBlock() {
            auto* block = new (m_context.GetAllocator()) BasicBlock(fmt::format("L{0}", ++m_blockCount));
            m_blocks.push_back(block);

            return block;
        }

        /**
         * @brief Change modifying block to new one from old one.
         * @param newBlock new @c BasicBlock to change.
         * @param sealBlock seal block if the parameter is true.
         * @return a @c BasicBlock previously modified.
         */
        BasicBlock* SetBlock(BasicBlock* newBlock, bool sealBlock = true) {
            auto* old = m_block;
            m_block = newBlock;

            if (sealBlock) {
                SealBlock(newBlock);
            }

            return old;
        }

        /**
         * @brief Allocate new IR register.
         * @return new @c IRValue.
         * @see BuildScript::IRValue
         */
        IRValue GetNewRegister() { return static_cast<IRValue>(m_index++); }

        /**
         * @brief Read constant.
         * @param pos a @c SourcePosition where the value positioned.
         * @param value a @c Value representing constant.
         * @return an @c IRValue representing value that read from the constant.
         *
         * This method loads constant @c value to newly allocated @c IRValue via LOAD_CONST op.
         * If @c value is already loaded other @c IRValue, the method return that @c IRValue.
         * If predecessors may have @c value, the method also return that @c IRValue if there's no conflict.
         * If the value conflicts, the method ignores it and allocates new @c IRValue.
         */
        IRValue ReadConst(SourcePosition pos, Value value);

        /**
         * @brief Read value from the symbol.
         * @param pos a @c SourcePosition where the symbol read.
         * @param symbol a @c Symbol to read.
         * @return an @c IRValue representing value that read from the symbol.
         *
         * This method loads symbol's value via LOAD_SYMBOL op if the symbol is not local.
         * Local symbols are just assigned directly into @c IRValue.
         * Method may insert SELECT op at branch junction to maintain static single assignment form.
         */
        IRValue ReadSymbol(SourcePosition pos, Symbol* symbol);

        /**
         * @brief Update value of the symbol.
         * @param pos a @c SourcePosition where the symbol written.
         * @param symbol a @c Symbol to write.
         * @param value an @c IRValue representing value that write to the symbol.
         */
        void WriteSymbol(SourcePosition pos, Symbol* symbol, IRValue value);

        /**
         * @brief Seal a block.
         * @param block a @c BasicBlock to seal.
         */
        void SealBlock(BasicBlock* block);

        /**
         * @brief Create new statement and insert it back of the block.
         * @tparam Stmt a type of the statement.
         * @tparam Args types of the parameters
         * @param args parameters that the type @c Stmt requires.
         */
        template <typename Stmt, typename... Args>
        Stmt* InsertBack(Args&&... args) {
            static_assert(std::is_base_of_v<IRStatement, Stmt>, "Stmt is not base of IRStatement.");

            auto* stmt = new (m_context.GetAllocator()) Stmt(std::forward<Args&&>(args)...);
            m_block->push_back(stmt);

            return stmt;
        }

        /**
         * @brief Get current @c BasicBlock.
         * @return current @c BasicBlock.
         */
        BasicBlock& GetCurrentBlock() { return *m_block; }

        /**
         * @brief Add exception handler information.
         * @param info an @c ExceptInfo represents exception handler.
         */
        void AddExceptInfo(ExceptInfo info) { m_handlers.push_back(std::move(info)); }
    }; // end class IRBuilder
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_IR_IRBUILDER_H