/*
 * ASTWalker.h
 * - .
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
     * @brief
     */
    enum class WalkerFlags {
        Continue,       //!< Continue walking on AST.
        SkipChildren,   //!< Continue but skip children.
        Abort           //!< Stop walking on AST.
    }; // end enum WalkerFlags

    /**
     * @brief
     */
    class ASTWalker {
    private:
        void Enter(const ASTNode*);
        void Leave(const ASTNode*);

    protected:
        virtual WalkerFlags OnEnterNode(const Parameters&) { return WalkerFlags::Continue; }
        virtual void OnLeaveNode(const Parameters&) {}

        virtual WalkerFlags OnEnterNode(const Label&) { return WalkerFlags::Continue; }
        virtual void OnLeaveNode(const Label&) {}

        #define V(name, kind) \
        virtual WalkerFlags OnEnterNode(const name&) { return WalkerFlags::Continue; } \
        virtual void OnLeaveNode(const name&) {}

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