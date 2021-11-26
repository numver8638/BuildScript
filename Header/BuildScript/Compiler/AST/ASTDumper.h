/*
 * ASTDumper.h
 * - Print AST in stdout.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_ASTDUMPER_H
#define BUILDSCRIPT_COMPILER_AST_ASTDUMPER_H

#include <BuildScript/Compiler/AST/ASTWalker.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    class SourceText;

    /**
     * @brief Print AST in stdout.
     */
    class ASTDumper final : public ASTWalker, NonCopyable {
    private:
        class ASTWriter;

        SourceText& m_source;
        ASTWriter* m_writer = nullptr;

        ASTWriter& writer() { return *m_writer; }

        WalkerFlags OnEnterNode(const Parameters&) override;
        void OnLeaveNode(const Parameters&) override;

        WalkerFlags OnEnterNode(const Label&) override;
        void OnLeaveNode(const Label&) override;

    #define V(name, _) \
        WalkerFlags OnEnterNode(const name&) override; \
        void OnLeaveNode(const name&) override;

        DECL_LIST(V)
        STMT_LIST(V)
        EXPR_LIST(V)
    #undef V

    public:
        explicit ASTDumper(SourceText& source)
            : m_source(source) {}

        /**
         * @brief Dump AST.
         * @param root root node of AST.
         */
        void Dump(const ASTNode* root);
    }; // end class ASTDumper
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ASTDUMPER_H