/*
 * IRCodeBlock.h
 * - Represents single executable unit represented in IR.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_IRCODEBLOCK_H
#define BUILDSCRIPT_COMPILER_IR_IRCODEBLOCK_H

#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace BuildScript {
    class BasicBlock;
    class Symbol;

    /**
     * @brief Exception handler table.
     */
    struct ExceptInfo {
        using HandlerInfo = std::tuple<BasicBlock*, Symbol*>;

        BasicBlock* Begin;
        BasicBlock* End;
        std::vector<HandlerInfo> Handlers;

        ExceptInfo(BasicBlock* B, BasicBlock* E)
            : Begin(B), End(E) {}
    };

    /**
     * @brief Represents single executable unit(function, method, initializer, etc.) represented in IR.
     */
    class IRCodeBlock final {
    private:
        std::string m_name;
        std::vector<BasicBlock*> m_blocks;

        const bool m_vararg;
        std::vector<ExceptInfo> m_handlers;
        std::vector<Symbol*> m_args;

    public:
        IRCodeBlock(std::string name, std::vector<BasicBlock*> blocks, bool vararg, std::vector<ExceptInfo> handlers,
                    std::vector<Symbol*> args)
            : m_name(std::move(name)), m_blocks(std::move(blocks)), m_vararg(vararg), m_handlers(std::move(handlers)),
              m_args(std::move(args)) {}

        std::string_view GetName() const { return m_name; }

        const std::vector<Symbol*>& GetArguments() const { return m_args; }

        int GetArgumentCount() const { return static_cast<int>(m_args.size()); }

        bool HasVariadicArgument() const { return m_vararg; }

        std::vector<ExceptInfo>& GetExceptHandlers() { return m_handlers; }

        std::vector<BasicBlock*>& GetBlocks() { return m_blocks; }
    }; // end class IRCodeBlock
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_IR_IRCODEBLOCK_H