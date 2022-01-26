/*
 * ASTWalker.h
 * - Iterate nodes in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_ASTWALKER_H
#define BUILDSCRIPT_COMPILER_AST_ASTWALKER_H

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/AST/Nodes.def>

namespace BuildScript {
#define V(name, kind) \
    class name;

    DECL_LIST(V)
    STMT_LIST(V)
    EXPR_LIST(V)
#undef V

    /**
     * @brief Iterate nodes in AST.
     */
    class ASTWalker {
    protected:
        virtual void Walk(const Parameters* node);
        virtual void Walk(const Label* node);

        virtual void Walk(const Declaration* node);
        virtual void Walk(const Statement* node);
        virtual void Walk(const Expression* node);

    #define V(name, kind) \
        virtual void Walk(const name* node);
        DECL_LIST(V)
        STMT_LIST(V)
        EXPR_LIST(V)

    #undef V

    public:
        virtual ~ASTWalker() = default;

        /**
         * @brief Iterate with given node.
         * @param node an @c ASTNode to walk.
         */
        void Walk(const ASTNode* node);
    }; // end class ASTWalker
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ASTWALKER_H