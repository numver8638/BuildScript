/*
 * ASTNode.h
 * - Base class of AST nodes.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_ASTNODE_H
#define BUILDSCRIPT_COMPILER_AST_ASTNODE_H

#include <BuildScript/Compiler/SourcePosition.h>

namespace BuildScript {
    class ASTVisitor; // Defined in <BuildScript/Compiler/AST/ASTVisitor.h>

    /**
     * @brief Base class of AST nodes.
     */
    class ASTNode {
    private:
        SourcePosition m_pos;

    protected:
        explicit ASTNode(const SourcePosition& pos)
                : m_pos(pos) {}

    public:
        const SourcePosition& GetPosition() const { return m_pos; }

        virtual ~ASTNode() = default;

        virtual void Accept(ASTVisitor*) const = 0;
    }; // end class ASTNode

    /**
     * @brief Base class of declarations in AST.
     */
    class Declaration : public ASTNode {
    protected:
        explicit Declaration(const SourcePosition& pos)
                : ASTNode(pos) {}
    }; // end class Declaration

    /**
     * @brief Base class of statements in AST.
     */
    class Statement : public ASTNode {
    protected:
        explicit Statement(const SourcePosition& pos)
                : ASTNode(pos) {}
    }; // end class Statement


    /**
     * @brief Represent kind of expression.
     */
    enum class ExpressionKind {
        Error,
        Ternary,
        Binary,
        Cast,
        Unary,
        Defined,
        Postfix,
        MemberAccess = Postfix,
        Invocation,
        Subscript,
        Primary,
        Literal = Primary,
        Map,
        List,
        Tuple,
        NamedTuple,
        Closure,
    }; // end enum ExpressionKind

    /**
     * @brief Base class of expressions in AST.
     */
    class Expression : public Statement {
    private:
        ExpressionKind m_kind;

    protected:
        Expression(const SourcePosition& pos, ExpressionKind kind)
                : Statement(pos), m_kind(kind) {}

    public:
        ExpressionKind GetKind() const { return m_kind; }
    }; // end class Expression
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ASTNODE_H