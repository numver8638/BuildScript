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
#define DECLARE_CLASS(name, kind) \
    class name;

    NODE_LIST(DECLARE_CLASS)
#undef DECLARE_CLASS

    /**
     * @brief Iterate nodes in AST.
     */
    class ASTWalker {
    protected:
        virtual void Walk(Parameters* node);
        virtual void Walk(Label* node);

        virtual void Walk(Declaration* node);
        virtual void Walk(Statement* node);
        virtual void Walk(Expression* node);

#define DECLARE_WALK(name, _) \
        virtual void Walk(name*);

    NODE_LIST(DECLARE_WALK)
#undef DECLARE_WALK

    public:
        virtual ~ASTWalker() = default;

        /**
         * @brief Iterate with given node.
         * @param node an @c ASTNode to walk.
         */
        void Walk(ASTNode* node);
    }; // end class ASTWalker

#define DEFINE_WALK(name, _) \
    void Walk(name*) override;
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ASTWALKER_H