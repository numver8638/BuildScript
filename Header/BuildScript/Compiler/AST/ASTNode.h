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
    class Parameters;
    class Label;

    class ASTIterator;

    /**
     * @brief Base class of all nodes in AST.
     */
    class ASTNode : protected ManagedObject {
        friend class ASTIterator;

    private:
        ASTKind m_kind;

    protected:
        /**
         * @brief
         * @param kind
         * @param range
         */
        explicit ASTNode(ASTKind kind)
            : m_kind(kind) {}

        /**
         * @brief Helper member function for iterating children.
         * @param index
         * @return a child node located in given index. `nullptr` if there's no children
         */
        virtual const ASTNode* GetChild(size_t index) const { return nullptr; }

    public:
        /**
         * @brief Get children of this node.
         * @return
         */
        ASTIterator GetChildren() const;

        bool IsDeclaration() const { return m_kind == ASTKind::Declaration; }

        const Declaration* AsDeclaration() const;

        Declaration* AsDeclaration() { return const_cast<Declaration*>(AsDeclaration()); }

        bool IsStatement() const { return m_kind == ASTKind::Statement; }

        const Statement* AsStatement() const;

        Statement* AsStatement() { return const_cast<Statement*>(AsStatement()); }

        bool IsExpression() const { return m_kind == ASTKind::Expression; }

        const Expression* AsExpression() const;

        Expression* AsExpression() { return const_cast<Expression*>(AsExpression()); }

        bool IsParameters() const { return m_kind == ASTKind::Parameters; }

        const Parameters* AsParameters() const;

        Parameters* AsParameters() { return const_cast<Parameters*>(AsParameters()); }

        bool IsLabel() const { return m_kind == ASTKind::Label; }

        const Label* AsLabel() const;

        Label* AsLabel() { return const_cast<Label*>(AsLabel()); }
    }; // end class ASTNode

    class ASTIterator {
    private:
        const ASTNode* m_target;

    public:
        struct iterator {
            friend ASTIterator;

        private:
            const ASTNode* m_target;
            size_t m_index = 0;
            const ASTNode* m_value = nullptr;

            iterator(const ASTNode* target, size_t index)
                : m_target(target), m_index(index) {
                m_value = m_target->GetChild(index);
            }

            iterator()
                : m_target(nullptr), m_index(0) {}

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = const ASTNode*;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;
            using const_reference = const value_type&;

            value_type operator *() const { return m_value; }

            bool operator !=(const iterator& other) const {
                return (m_value != other.m_value);
            }

            iterator& operator ++() {
                m_value = m_target->GetChild(++m_index);
                return *this;
            }

            iterator operator ++(int) {
                auto old = *this;
                operator ++();

                return old;
            }
        };

        using const_iterator = iterator;

        explicit ASTIterator(const ASTNode* node)
            : m_target(node) {}

        ASTIterator(const ASTNode* node, size_t begin, size_t end = static_cast<size_t>(-1));

        const_iterator begin() const { return { m_target, 0 }; }
        const_iterator end() const { return {}; }
    }; // end class ASTIterator

    inline ASTIterator ASTNode::GetChildren() const { return ASTIterator(this); }

    /**
     * @brief .
     */
    enum class DeclarationKind;

    /**
     * @brief Represent declaration in AST.
     */
    class Declaration : public ASTNode {
    private:
        DeclarationKind m_kind;

    protected:
        /**
         * @brief
         * @param kind
         * @param range
         */
        explicit Declaration(DeclarationKind kind)
            : ASTNode(ASTKind::Declaration), m_kind(kind) {}

    public:
        /**
         * @brief
         * @return
         */
        DeclarationKind GetKind() const { return m_kind; }

        /**
         * @brief
         * @tparam DeclType
         * @return
         */
        template <typename DeclType>
        const DeclType* As() const {
            static_assert(std::is_base_of_v<Declaration, DeclType>, "DeclType is not base of Declaration.");

            return (DeclType::Kind == m_kind) ? static_cast<const DeclType*>(this) : nullptr;
        }

        /**
         * @brief
         * @tparam DeclType
         * @return
         */
        template <typename DeclType>
        DeclType* As() {
            static_assert(std::is_base_of_v<Declaration, DeclType>, "DeclType is not base of Declaration.");

            return (DeclType::Kind == m_kind) ? static_cast<DeclType*>(this) : nullptr;
        }
    }; // end class Declaration

    inline const Declaration* ASTNode::AsDeclaration() const {
        return IsDeclaration() ? static_cast<const Declaration*>(this) : nullptr; // NOLINT
    }

    /**
     * @brief .
     */
    enum class StatementKind;

    /**
     * @brief Represents statement in AST.
     */
    class Statement : public ASTNode {
    private:
        StatementKind m_kind;

    protected:
        /**
         * @brief
         * @param kind
         * @param range
         */
        explicit Statement(StatementKind kind)
            : ASTNode(ASTKind::Statement), m_kind(kind) {}

    public:
        /**
         * @brief
         * @return
         */
        StatementKind GetKind() const { return m_kind; }

        /**
         * @brief
         * @tparam StmtType
         * @return
         */
        template <typename StmtType>
        const StmtType* As() const {
            static_assert(std::is_base_of_v<Statement, StmtType>, "StmtType is not base of Statement.");

            return (StmtType::Kind == m_kind) ? static_cast<const StmtType*>(this) : nullptr;
        }

        /**
         * @brief
         * @tparam StmtType
         * @return
         */
        template <typename StmtType>
        StmtType* As() {
            static_assert(std::is_base_of_v<Statement, StmtType>, "StmtType is not base of Statement.");

            return (StmtType::Kind == m_kind) ? static_cast<StmtType*>(this) : nullptr;
        }
    }; // end class Statement

    inline const Statement* ASTNode::AsStatement() const {
        return IsStatement() ? static_cast<const Statement*>(this) : nullptr; // NOLINT
    }

    /**
     * @brief .
     */
    enum class ExpressionKind;

    /**
     * @brief Represent expression in AST.
     */
    class Expression : public ASTNode {
    private:
        ExpressionKind m_kind;

    protected:
        /**
         * @brief
         * @param kind
         * @param range
         */
        explicit Expression(ExpressionKind kind)
            : ASTNode(ASTKind::Expression), m_kind(kind) {}

    public:
        /**
         * @brief
         * @return
         */
        ExpressionKind GetKind() const { return m_kind; }

        /**
         * @brief
         * @tparam ExprType
         * @return
         */
        template <typename ExprType>
        const ExprType* As() const {
            static_assert(std::is_base_of_v<Expression, ExprType>, "ExprType is not base of Expression.");

            return (ExprType::Kind == m_kind) ? static_cast<const ExprType*>(this) : nullptr;
        }

        /**
         * @brief
         * @tparam ExprType
         * @return
         */
        template <typename ExprType>
        ExprType* As() {
            static_assert(std::is_base_of_v<Expression, ExprType>, "ExprType is not base of Expression.");

            return (ExprType::Kind == m_kind) ? static_cast<ExprType*>(this) : nullptr;
        }
    }; // end class Expression

    inline const Expression* ASTNode::AsExpression() const {
        return IsExpression() ? static_cast<const Expression*>(this) : nullptr; // NOLINT
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ASTNODE_H