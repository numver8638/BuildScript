/*
 * ASTNode.h
 * - Base class of nodes in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_ASTNODE_H
#define BUILDSCRIPT_COMPILER_AST_ASTNODE_H

#include <array>
#include <type_traits>

#include <BuildScript/Compiler/ManagedObject.h>
#include <BuildScript/Compiler/SourcePosition.h>

namespace BuildScript {
    /**
     * @brief Represents kind of node in AST.
     */
    enum class ASTKind {
        Declaration,
        Statement,
        Expression,
        Parameters,
        Label
    }; // end enum ASTKind

    class Declaration;
    class Statement;
    class Expression;
    class Parameters; // Defined in <BuildScript/Compiler/AST/Parameters.h>
    class Label; // Defined in <BuildScript/Compiler/AST/Statements.h>

    /**
     * @brief Base class of all nodes in AST.
     */
    class ASTNode : protected ManagedObject {
    private:
        ASTKind m_kind;

    protected:
        /**
         * @brief Constructor.
         * @param kind a @c ASTKind that represents kind of node in AST.
         */
        explicit ASTNode(ASTKind kind)
            : m_kind(kind) {}

    public:
        /**
         * @brief Check whether this node is @c Declaration or not.
         * @return @c true if the node is @c Declaration otherwise @c false.
         */
        bool IsDeclaration() const { return m_kind == ASTKind::Declaration; }

        /**
         * @brief Cast the node to @c Declaration.
         * @return a node casted to @c Declaration if this is @c Declaration otherwise returns @c nullptr.
         */
        const Declaration* AsDeclaration() const;

        /**
         * @brief Cast the node to @c Declaration.
         * @return a node casted to @c Declaration if this is @c Declaration otherwise returns @c nullptr.
         */
        Declaration* AsDeclaration();

        /**
         * @brief Check whether this node is @c Statement or not.
         * @return @c true if the node is @c Statement otherwise @c false.
         */
        bool IsStatement() const { return m_kind == ASTKind::Statement; }

        /**
         * @brief Cast the node to @c Statement.
         * @return a node casted to @c Statement if this is @c Statement otherwise returns @c nullptr.
         */
        const Statement* AsStatement() const;

        /**
         * @brief Cast the node to @c Statement.
         * @return a node casted to @c Statement if this is @c Statement otherwise returns @c nullptr.
         */
        Statement* AsStatement();

        /**
         * @brief Check whether this node is @c Expression or not.
         * @return @c true if the node is @c Expression otherwise @c false.
         */
        bool IsExpression() const { return m_kind == ASTKind::Expression; }

        /**
         * @brief Cast the node to @c Expression.
         * @return a node casted to @c Expression if this is @c Expression otherwise returns @c nullptr.
         */
        const Expression* AsExpression() const;

        /**
         * @brief Cast the node to @c Expression.
         * @return a node casted to @c Expression if this is @c Expression otherwise returns @c nullptr.
         */
        Expression* AsExpression();

        /**
         * @brief Check whether this node is @c Parameters or not.
         * @return @c true if the node is @c Parameters otherwise @c false.
         */
        bool IsParameters() const { return m_kind == ASTKind::Parameters; }

        /**
         * @brief Cast the node to @c Parameters.
         * @return a node casted to @c Parameters if this is @c Parameters otherwise returns @c nullptr.
         */
        const Parameters* AsParameters() const;

        /**
         * @brief Cast the node to @c Parameters.
         * @return a node casted to @c Parameters if this is @c Parameters otherwise returns @c nullptr.
         */
        Parameters* AsParameters();

        /**
         * @brief Check whether this node is @c Label or not.
         * @return @c true if the node is @c Label otherwise @c false.
         */
        bool IsLabel() const { return m_kind == ASTKind::Label; }

        /**
         * @brief Cast the node to @c Label.
         * @return a node casted to @c Label if this is @c Label otherwise returns @c nullptr.
         */
        const Label* AsLabel() const;

        /**
         * @brief Cast the node to @c Label.
         * @return a node casted to @c Label if this is @c Label otherwise returns @c nullptr.
         */
        Label* AsLabel();
    }; // end class ASTNode

    /**
     * @brief Represents kind of declaration.
     */
    enum class DeclarationKind; // Defined in <BuildScript/Compiler/AST/Declarations.h>

    /**
     * @brief Represent declaration in AST.
     */
    class Declaration : public ASTNode {
    private:
        DeclarationKind m_kind;

    protected:
        /**
         * @brief Constructor.
         * @param kind a @c DeclarationKind that represents kind of declaration.
         */
        explicit Declaration(DeclarationKind kind)
            : ASTNode(ASTKind::Declaration), m_kind(kind) {}

    public:
        /**
         * @brief Get kind of the declaration.
         * @return a @c DeclarationKind that represents kind of declaration.
         */
        DeclarationKind GetKind() const { return m_kind; }

        /**
         * @brief Cast the declaration to derived type @c DeclType.
         * @tparam DeclType the type which is derived from @c Declaration.
         * @return the declaration casted to @c DeclType if this is @c DeclType otherwise returns @c nullptr.
         */
        template <typename DeclType>
        const DeclType* As() const {
            static_assert(std::is_base_of_v<Declaration, DeclType>, "DeclType is not base of Declaration.");

            return (DeclType::Kind == m_kind) ? static_cast<const DeclType*>(this) : nullptr;
        }

        /**
         * @brief Cast the declaration to derived type @c DeclType.
         * @tparam DeclType the type which is derived from @c Declaration.
         * @return the declaration casted to @c DeclType if this is @c DeclType otherwise returns @c nullptr.
         */
        template <typename DeclType>
        DeclType* As() {
            static_assert(std::is_base_of_v<Declaration, DeclType>, "DeclType is not base of Declaration.");

            return (DeclType::Kind == m_kind) ? static_cast<DeclType*>(this) : nullptr;
        }
    }; // end class Declaration

    inline Declaration* ASTNode::AsDeclaration() {
        return IsDeclaration() ? static_cast<Declaration*>(this) : nullptr; // NOLINT
    }

    inline const Declaration* ASTNode::AsDeclaration() const {
        return IsDeclaration() ? static_cast<const Declaration*>(this) : nullptr; // NOLINT
    }

    /**
     * @brief Represents kind of statement.
     */
    enum class StatementKind; // Defined in <BuildScript/Compiler/AST/Statements.h>

    /**
     * @brief Represents statement in AST.
     */
    class Statement : public ASTNode {
    private:
        StatementKind m_kind;

    protected:
        /**
         * @brief Constructor.
         * @param kind a @c StatementKind that represents kind of statement.
         */
        explicit Statement(StatementKind kind)
            : ASTNode(ASTKind::Statement), m_kind(kind) {}

    public:
        /**
         * @brief Get kind of the statement.
         * @return a @c StatementKind that represents kind of statement.
         */
        StatementKind GetKind() const { return m_kind; }

        /**
         * @brief Cast the statement to derived type @c StmtType.
         * @tparam StmtType the type which is derived from @c Statement.
         * @return the statement casted to @c StmtType if this is @c StmtType otherwise returns @c nullptr.
         */
        template <typename StmtType>
        const StmtType* As() const {
            static_assert(std::is_base_of_v<Statement, StmtType>, "StmtType is not base of Statement.");

            return (StmtType::Kind == m_kind) ? static_cast<const StmtType*>(this) : nullptr;
        }

        /**
         * @brief Cast the statement to derived type @c StmtType.
         * @tparam StmtType the type which is derived from @c Statement.
         * @return the statement casted to @c StmtType if this is @c StmtType otherwise returns @c nullptr.
         */
        template <typename StmtType>
        StmtType* As() {
            static_assert(std::is_base_of_v<Statement, StmtType>, "StmtType is not base of Statement.");

            return (StmtType::Kind == m_kind) ? static_cast<StmtType*>(this) : nullptr;
        }
    }; // end class Statement

    inline Statement* ASTNode::AsStatement() {
        return IsStatement() ? static_cast<Statement*>(this) : nullptr; // NOLINT
    }

    inline const Statement* ASTNode::AsStatement() const {
        return IsStatement() ? static_cast<const Statement*>(this) : nullptr; // NOLINT
    }

    /**
     * @brief Represents kind of expression.
     */
    enum class ExpressionKind; // Defined in <BuildScript/Compiler/AST/Expressions.h>

    /**
     * @brief Represent expression in AST.
     */
    class Expression : public ASTNode {
    private:
        ExpressionKind m_kind;

    protected:
        /**
         * @brief Constructor.
         * @param kind a @c ExpressionKind that represents kind of expression.
         */
        explicit Expression(ExpressionKind kind)
            : ASTNode(ASTKind::Expression), m_kind(kind) {}

    public:
        /**
         * @brief Get kind of the expression.
         * @return a @c ExpressionKind that represents kind of expression.
         */
        ExpressionKind GetKind() const { return m_kind; }

        /**
         * @brief Cast the expression to derived type @c ExprType.
         * @tparam ExprType the type which is derived from @c Expression.
         * @return the expression casted to @c ExprType if this is @c ExprType otherwise returns @c nullptr.
         */
        template <typename ExprType>
        const ExprType* As() const {
            static_assert(std::is_base_of_v<Expression, ExprType>, "ExprType is not base of Expression.");

            return (ExprType::Kind == m_kind) ? static_cast<const ExprType*>(this) : nullptr;
        }

        /**
         * @brief Cast the expression to derived type @c ExprType.
         * @tparam ExprType the type which is derived from @c Expression.
         * @return the expression casted to @c ExprType if this is @c ExprType otherwise returns @c nullptr.
         */
        template <typename ExprType>
        ExprType* As() {
            static_assert(std::is_base_of_v<Expression, ExprType>, "ExprType is not base of Expression.");

            return (ExprType::Kind == m_kind) ? static_cast<ExprType*>(this) : nullptr;
        }
    }; // end class Expression

    inline Expression* ASTNode::AsExpression() {
        return IsExpression() ? static_cast<Expression*>(this) : nullptr; // NOLINT
    }

    inline const Expression* ASTNode::AsExpression() const {
        return IsExpression() ? static_cast<const Expression*>(this) : nullptr; // NOLINT
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ASTNODE_H