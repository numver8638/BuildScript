/*
 * Expressions.h
 * - Represent expression in AST.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_EXPRESSIONS_H
#define BUILDSCRIPT_COMPILER_AST_EXPRESSIONS_H

#include <map>
#include <utility>
#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Compiler/StringRef.h>

namespace BuildScript {
    class ErrorExpression final : public Expression {
    public:
        explicit ErrorExpression(const SourcePosition& pos)
            : Expression(pos, ExpressionKind::Error) {}

        void Accept(ASTVisitor*) const override;
    }; // end class ErrorExpression

    class TernaryExpression final : public Expression {
    private:
        Expression* m_Tvalue;
        Expression* m_condition;
        Expression* m_Fvalue;

    public:
        TernaryExpression(Expression* Tval, Expression* cond, Expression* Fval)
            : Expression(Tval->GetPosition(), ExpressionKind::Ternary), m_Tvalue(Tval), m_condition(cond), m_Fvalue(Fval) {}

        ~TernaryExpression() final {
            delete m_Tvalue;
            delete m_condition;
            delete m_Fvalue;
        }

        const Expression* GetCondition() const { return m_condition; }

        const Expression* GetValueOnTrue() const { return m_Tvalue; }

        const Expression* GetValueOnFalse() const { return m_Fvalue; }

        void Accept(ASTVisitor*) const override;
    }; // end class TernaryExpression

    enum class BinaryOp {
        Mul,
        Div,
        Mod,
        Add,
        Sub,
        LeftShift,
        RightShift,
        BitAnd,
        BitXor,
        BitOr,
        Less,
        LessOrEqual,
        Grater,
        GraterOrEqual,
        In,
        NotIn,
        Equal,
        NotEqual,
        Is,
        IsNot,
        LogicalAnd,
        LogicalOr
    }; // end enum BinaryOp

    class BinaryExpression final : public Expression {
    private:
        Expression* m_left;
        Expression* m_right;
        BinaryOp m_op;

    public:
        BinaryExpression(Expression* left, Expression* right, BinaryOp op)
            : Expression(left->GetPosition(), ExpressionKind::Binary), m_left(left), m_right(right), m_op(op){}

        ~BinaryExpression() final {
            delete m_left;
            delete m_right;
        }

        BinaryOp GetOperator() const { return m_op; }

        const Expression* GetLeft() const { return m_left; }

        const Expression* GetRight() const { return m_right; }

        void Accept(ASTVisitor*) const override;
    }; // end class BinaryExpression

    enum class UnaryOp {
        Identity,
        Negate,
        BinaryNot,
        LogicalNot
    }; // end enum UnaryOp

    class UnaryExpression final : public Expression {
    private:
        Expression* m_expr;
        UnaryOp m_op;

    public:
        UnaryExpression(const SourcePosition& pos, Expression* expr, UnaryOp op)
            : Expression(pos, ExpressionKind::Unary), m_expr(expr), m_op(op) {}

        ~UnaryExpression() final {
            delete m_expr;
        }

        UnaryOp GetOperator() const { return m_op; }

        const Expression* GetExpression() const { return m_expr; }

        void Accept(ASTVisitor*) const override;
    }; // end class UnaryExpression

    class DefinedExpression final : public Expression {
    private:
        StringRef m_id;
        Expression* m_target;

    public:
        DefinedExpression(const SourcePosition& pos, const StringRef& id, Expression* target)
            : Expression(pos, ExpressionKind::Defined), m_id(id), m_target(target) {}

        ~DefinedExpression() final {
            delete m_target;
        }

        const StringRef& GetID() const { return m_id; }

        const Expression* GetTarget() const { return m_target; }

        void Accept(ASTVisitor*) const override;
    }; // end class DefinedExpression

    class CastExpression final : public Expression {
    private:
        Expression* m_target;
        StringRef m_typename;

    public:
        CastExpression(Expression* target, const StringRef& _typename)
            : Expression(target->GetPosition(), ExpressionKind::Cast), m_target(target), m_typename(_typename) {}

        ~CastExpression() final {
            delete m_target;
        }

        const StringRef& GetTypeName() const { return m_typename; }

        const Expression* GetTarget() const { return m_target; }

        void Accept(ASTVisitor*) const override;
    }; // end class CastExpression

    class MemberAccessExpression final : public Expression {
    private:
        Expression* m_target;
        StringRef m_member;

    public:
        MemberAccessExpression(Expression* target, const StringRef& member)
            : Expression(target->GetPosition(), ExpressionKind::MemberAccess), m_target(target), m_member(member) {}

        ~MemberAccessExpression() final {
            delete m_target;
        }

        const StringRef& GetMemberName() const { return m_member; }

        const Expression* GetTarget() const { return m_target; }

        void Accept(ASTVisitor*) const override;
    }; // end class MemberAccessExpression

    class SubscriptExpression final : public Expression {
    private:
        Expression* m_target;
        Expression* m_index;

    public:
        SubscriptExpression(Expression* target, Expression* index)
            : Expression(target->GetPosition(), ExpressionKind::Subscript), m_target(target), m_index(index) {}

        ~SubscriptExpression() final {
            delete m_target;
            delete m_index;
        }

        const Expression* GetIndex() const { return m_index; }

        const Expression* GetTarget() const { return m_target; }

        void Accept(ASTVisitor*) const override;
    }; // end class SubscriptExpression

    class InvocationExpression final : public Expression {
    private:
        Expression* m_target;
        std::vector<Expression*> m_args;

    public:
        InvocationExpression(Expression* target, std::vector<Expression*>&& args)
            : Expression(target->GetPosition(), ExpressionKind::Invocation), m_target(target), m_args(std::move(args)) {}

        ~InvocationExpression() final {
            delete m_target;
            for (auto e : m_args)
                delete e;
        }

        const Expression* GetTarget() const { return m_target; }

        const std::vector<Expression*>& GetArguments() const { return m_args; }

        void Accept(ASTVisitor*) const override;
    }; // end class InvocationExpression

    enum class LiteralKind {
        Identifier,
        Integer,
        Float,
        String,
        True,
        False,
        None,
        Self,
        Super,
    }; // end enum LiteralKind

    class LiteralExpression : public Expression {
    private:
        LiteralKind m_kind;
        StringRef m_image;

    public:
        LiteralExpression(const SourcePosition& pos, LiteralKind kind, const StringRef& image)
            : Expression(pos, ExpressionKind::Literal), m_kind(kind), m_image(image) {}

        const StringRef& GetImage() const { return m_image; }

        LiteralKind GetLiteralKind() const { return m_kind; }

        void Accept(ASTVisitor*) const override;
    }; // end class LiteralExpression

    class StringLiteral final : public LiteralExpression {
    private:
        std::string m_text;
        std::vector<Expression*> m_exprs;

    public:
        StringLiteral(const SourcePosition& pos, std::string&& text, std::vector<Expression*>&& exprs)
            : LiteralExpression(pos, LiteralKind::String, StringRef()), m_text(std::move(text)), m_exprs(std::move(exprs)) {}

        ~StringLiteral() final {
            for (auto e : m_exprs)
                delete e;
        }

        const std::string& GetText() const { return m_text; }

        const std::vector<Expression*>& GetInterpolatedExpressions() const { return m_exprs; }

        void Accept(ASTVisitor*) const override;
    }; // end class StringLiteral

    class MapExpression final : public Expression {
    private:
        std::map<Expression*, Expression*> m_items;

    public:
        MapExpression(const SourcePosition& pos, std::map<Expression*, Expression*>&& items)
            : Expression(pos, ExpressionKind::Map), m_items(std::move(items)) {}

        const std::map<Expression*, Expression*>& GetItems() const { return m_items; }

        void Accept(ASTVisitor*) const override;
    }; // end class MapExpression

    class ListExpression final : public Expression {
    private:
        std::vector<Expression*> m_items;

    public:
        ListExpression(const SourcePosition& pos, std::vector<Expression*>&& items)
            : Expression(pos, ExpressionKind::List), m_items(std::move(items)) {}

        const std::vector<Expression*>& GetItems() const { return m_items; }

        void Accept(ASTVisitor*) const override;
    }; // end class ListExpression

    class TupleExpression final : public Expression {
    private:
        std::vector<Expression*> m_items;

    public:
        TupleExpression(const SourcePosition& pos, std::vector<Expression*>&& items)
            : Expression(pos, ExpressionKind::Tuple), m_items(std::move(items)) {}

        const std::vector<Expression*>& GetItems() const { return m_items; }

        void Accept(ASTVisitor*) const override;
    }; // end class TupleExpression

    class NamedTupleExpression final : public Expression {
    private:
        std::map<Expression*, Expression*> m_items;

    public:
        NamedTupleExpression(const SourcePosition& pos, std::map<Expression*, Expression*>&& items)
            : Expression(pos, ExpressionKind::NamedTuple), m_items(std::move(items)) {}

        const std::map<Expression*, Expression*>& GetItems() const { return m_items; }

        void Accept(ASTVisitor*) const override;
    }; // end class NamedTupleExpression

    class ClosureExpression final : public Expression {
    public:
        using Parameter = std::pair<StringRef, SourcePosition>;
    private:
        std::vector<Parameter> m_params;
        ASTNode* m_body;

    public:
        ClosureExpression(const SourcePosition& pos, std::vector<Parameter>&& params, ASTNode* body)
            : Expression(pos, ExpressionKind::Closure), m_params(std::move(params)), m_body(body) {}

        const std::vector<Parameter>& GetParameters() const { return m_params; }

        const ASTNode* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    }; // end class ClosureExpression
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_EXPRESSIONS_H