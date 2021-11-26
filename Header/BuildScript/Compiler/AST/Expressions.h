/*
 * Expressions.h
 * - Represents expressions in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_EXPRESSIONS_H
#define BUILDSCRIPT_COMPILER_AST_EXPRESSIONS_H

#include <array>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Utils/TrailObjects.h>

namespace BuildScript {
    class Parameters;

    enum class ExpressionKind {
        Invalid,

        Pass,       // Used for empty closure.
        KeyValue,   // Used for map expression

        Ternary,
        Binary,

        // Unary expressions
        Unary,
        Defined,
        Raise,

        // Postfix expressions
        MemberAccess,
        Invocation,
        Subscript,

        // Primary expressions
        Parenthesized,
        List,
        Map,
        Closure,
        Literal
    }; // end enum ExpressionKind

    /**
     * @brief Represents invalid expression.
     */
    class InvalidExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Invalid;

    private:
        explicit InvalidExpression(SourceRange range)
            : Expression(Kind, range) {}

    public:
        static InvalidExpression* Create(Context& context, SourceRange range);
    }; // end class InvalidExpression

    /**
     * @brief Represents pass keyword in closure.
     */
    class PassExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Pass;

    private:
        explicit PassExpression(SourceRange range)
            : Expression(Kind, range) {}

    public:
        static PassExpression* Create(Context& context, SourceRange range);
    }; // end class PassExpression

    /**
     * @brief Represents ternary expression.
     */
    class TernaryExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Ternary;

    private:
        Expression* m_valueT;
        SourcePosition m_if;
        Expression* m_cond;
        SourcePosition m_else;
        Expression* m_valueF;

        TernaryExpression(SourceRange range, Expression* valueT, SourcePosition _if, Expression* cond,
                          SourcePosition _else, Expression* valueF)
            : Expression(Kind, range), m_valueT(valueT), m_if(_if), m_cond(cond), m_else(_else), m_valueF(valueF) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get expression when condition is true.
         * @return
         */
        const Expression* GetValueOnTrue() const { return m_valueT; }

        /**
         * @brief Get a position of 'if' keyword.
         * @return a @c SourcePosition representing where 'if' keyword positioned.
         */
        SourcePosition GetIfPosition() const { return m_if; }

        /**
         * @brief Get an expression represents condition.
         * @return
         */
        const Expression* GetCondition() const { return m_cond; }

        /**
         * @brief Get a position of 'else' keyword.
         * @return a @c SourcePosition representing where 'else' keyword positioned.
         */
        SourcePosition GetElsePosition() const { return m_else; }

        /**
         * @brief Get an expression when condition is false.
         * @return
         */
        const Expression* GetValueOnFalse() const { return m_valueF; }

        static TernaryExpression*
        Create(Context& context, Expression* valueT, SourcePosition _if, Expression* condition, SourcePosition _else,
               Expression* valueF);
    }; // end class TernaryExpression

    /**
     * @brief Represents binary operator.
     */
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
        Is,
        IsNot,
        In,
        NotIn,
        Equal,
        NotEqual,
        LogicalAnd,
        LogicalOr
    }; // end enum BinaryOp

    /**
     * @brief Represents binary expression.
     */
    class BinaryExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Binary;

    private:
        Expression* m_left;
        BinaryOp m_op;
        std::array<SourcePosition, 2> m_pos;
        Expression* m_right;

        BinaryExpression(SourceRange range, Expression* left, BinaryOp op, std::array<SourcePosition, 2> pos,
                         Expression* right)
            : Expression(Kind, range), m_left(left), m_op(op), m_pos(pos), m_right(right) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get left hand side expression.
         * @return
         */
        const Expression* GetLeft() const { return m_left; }

        /**
         * @brief
         * @return
         */
        BinaryOp GetOp() const { return m_op; }

        /**
         * @brief Get a position of first operator.
         * @return a @c SourcePosition representing where first operator positioned.
         */
        const SourcePosition& GetFirstOpPosition() const { return m_pos[0]; }

        /**
         * @brief Get a position of second operator.
         * @return a @c SourcePosition representing where second operator positioned.
         */
        const SourcePosition& GetSecondOpPosition() const { return m_pos[1]; }

        /**
         * @brief Get right hand side expression.
         * @return
         */
        const Expression* GetRight() const { return m_right; }

        static BinaryExpression*
        Create(Context& context, Expression* left, BinaryOp op, std::array<SourcePosition, 2> pos, Expression* right);
    }; // end class BinaryExpression

    /**
     * @brief
     */
    enum class UnaryOp {
        Identity,
        Negate,
        LogicalNot,
        BinaryNot
    }; // end enum UnaryOp

    /**
     * @brief Represents unary expression.
     */
    class UnaryExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Unary;

    private:
        UnaryOp m_op;
        SourcePosition m_pos;
        Expression* m_expr;

        UnaryExpression(SourceRange range, UnaryOp op, SourcePosition pos, Expression* expr)
            : Expression(Kind, range), m_op(op), m_pos(pos), m_expr(expr) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @return
         */
        UnaryOp GetOp() const { return m_op; }

        /**
         * @brief Get a position of the operator.
         * @return a @c SourcePosition representing where the operator positioned.
         */
        SourcePosition GetOpPosition() const { return m_pos; }

        /**
         * @brief
         * @return
         */
        const Expression* GetExpression() const { return m_expr; }

        static UnaryExpression* Create(Context& context, UnaryOp op, SourcePosition pos, Expression* expr);
    }; // end class UnaryExpression

    /**
     * @brief Represents defined expression.
     */
    class DefinedExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Defined;

    private:
        SourcePosition m_defined;
        Identifier m_id;
        SourcePosition m_in;
        Expression* m_target;

        DefinedExpression(SourceRange range, SourcePosition defined, Identifier id, SourcePosition in,
                          Expression* target)
            : Expression(Kind, range), m_defined(defined), m_id(std::move(id)), m_in(in), m_target(target) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'defined' keyword.
         * @return a @c SourcePosition representing where 'defined' keyword positioned.
         */
        SourcePosition GetDefinedPosition() const { return m_defined; }

        /**
         * @brief
         * @return
         */
        const Identifier& GetID() const { return m_id; }

        /**
         * @brief Get a position of 'in' keyword.
         * @return a @c SourcePosition representing where 'in' keyword positioned.
         */
        SourcePosition GetInPosition() const { return m_in; }

        /**
         * @brief
         * @return
         */
        const Expression* GetTarget() const { return m_target; }

        static DefinedExpression*
        Create(Context& context, SourcePosition defined, Identifier id, SourcePosition in, Expression* target);
    }; // end class DefinedExpression

    /**
     * @brief Represents raise expression.
     */
    class RaiseExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Raise;

    private:
        SourcePosition m_raise;
        Expression* m_target;

        RaiseExpression(SourceRange range, SourcePosition raise, Expression* target)
            : Expression(Kind, range), m_raise(raise), m_target(target) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'raise' keyword.
         * @return a @c SourcePosition representing where 'raise' keyword positioned.
         */
        SourcePosition GetRaisePosition() const { return m_raise; }

        /**
         * @brief
         * @return
         */
        const Expression* GetTarget() const { return m_target; }

        static RaiseExpression* Create(Context& context, SourcePosition raise, Expression* expr);
    }; // end class RaiseExpression

    /**
     * @brief Represents member access expression.
     */
    class MemberAccessExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::MemberAccess;

    private:
        Expression* m_target;
        SourcePosition m_dot;
        Identifier m_name;

        MemberAccessExpression(SourceRange range, Expression* target, SourcePosition dot, Identifier name)
            : Expression(Kind, range), m_target(target), m_dot(dot), m_name(std::move(name)) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @return
         */
        const Expression* GetTarget() const { return m_target; }

        /**
         * @brief Get a position of '.'.
         * @return a @c SourcePosition representing where '.' positioned.
         */
        SourcePosition GetDotPosition() const { return m_dot; }

        /**
         * @brief Get name of ...
         * @return
         */
        const Identifier& GetMemberName() const { return m_name; }

        static MemberAccessExpression*
        Create(Context& context, Expression* target, SourcePosition dot, Identifier name);
    }; // end class MemberAccessExpression

    /**
     * @brief Represent invocation(function or method call) expression.
     */
    class InvocationExpression final : public Expression,
                                       TrailObjects<InvocationExpression, Expression*, SourcePosition> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = ExpressionKind::Invocation;

    private:
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_exprCount;
        size_t m_commaCount;

        InvocationExpression(SourceRange range, SourcePosition open, SourcePosition close, size_t exprCount,
                             size_t commaCount)
            : Expression(Kind, range), m_open(open), m_close(close), m_exprCount(exprCount), m_commaCount(commaCount) {}

        size_t GetTrailCount(OverloadToken<Expression*>) const { return m_exprCount; } // TrailObjects support.
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return m_commaCount; } // TrailObjects support.

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @return
         */
        Expression* GetTarget() const { return At<Expression*>(0); }

        /**
         * @brief Get a position of '('.
         * @return a @c SourcePosition representing where '(' positioned.
         */
        SourcePosition GetOpenParenPosition() const { return m_open; }

        /**
         * @brief
         * @return
         */
        ASTIterator GetArguments() const;

        /**
         * @brief Get count of arguments.
         * @return a count of arguments.
         */
        size_t GetArguementCount() const { return m_exprCount - 1; }

        /**
         * @brief
         * @return
         */
        TrailIterator<const SourcePosition> GetCommaPositions() const { return GetTrailObjects<SourcePosition>(); }

        /**
         * @brief
         * @param index
         * @return a @c SourcePosition representing where ',' positioned.
         */
        SourcePosition GetCommaPositionAt(size_t index) const { return At<SourcePosition>(index); }

        /**
         * @brief Get a position of ')'.
         * @return a @c SourcePosition representing where ')' positioned.
         */
        SourcePosition GetCloseParenPosition() const { return m_close; }

        static InvocationExpression*
        Create(Context& context, SourcePosition open, const std::vector<Expression*>& nodes,
               const std::vector<SourcePosition>& commas, SourcePosition close);
    }; // end class InvocationExpression

    /**
     * @brief Represents subscript expression.
     */
    class SubscriptExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Subscript;

    private:
        Expression* m_target;
        SourcePosition m_open;
        Expression* m_index;
        SourcePosition m_close;

        SubscriptExpression(SourceRange range, Expression* target, SourcePosition open, Expression* index,
                            SourcePosition close)
            : Expression(Kind, range), m_target(target), m_open(open), m_index(index), m_close(close) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @return
         */
        Expression* GetTarget() const { return m_target; }

        /**
         * @brief Get a position of '['.
         * @return a @c SourcePosition representing where '[' positioned.
         */
        SourcePosition GetOpenSquarePosition() const { return m_open; }

        /**
         * @brief
         * @return
         */
        Expression* GetIndex() const { return m_index; }

        /**
         * @brief Get a position of ']'.
         * @return a @c SourcePosition representing where ']' positioned.
         */
        SourcePosition GetCloseSquarePosition() const { return m_close; }

        static SubscriptExpression*
        Create(Context& context, Expression* target, SourcePosition open, Expression* index, SourcePosition close);
    }; // end class SubscriptExpression

    /**
     * @brief Represents expression surrounded with parens.
     */
    class ParenthesizedExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Parenthesized;

    private:
        SourcePosition m_open;
        SourcePosition m_close;
        Expression* m_expr;

        ParenthesizedExpression(SourceRange range, SourcePosition open, SourcePosition close, Expression* expr)
            : Expression(Kind, range), m_open(open), m_close(close), m_expr(expr) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of '('.
         * @return a @c SourcePosition representing where '(' positioned.
         */
        SourcePosition GetOpenParenPosition() const { return m_open; }

        /**
         * @brief Get a position of ')'.
         * @return a @c SourcePosition representing where ')' positioned.
         */
        SourcePosition GetCloseParenPosition() const { return m_close; }

        /**
         * @brief
         * @return
         */
        Expression* GetInnerExpression() const { return m_expr; }

        static ParenthesizedExpression*
        Create(Context& context, SourcePosition open, Expression* index, SourcePosition close);
    }; // end class ParenthesizedExpression

    /**
     * @brief Represents list.
     */
    class ListExpression final : public Expression, TrailObjects<ListExpression, Expression*, SourcePosition> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = ExpressionKind::List;

    private:
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        ListExpression(SourceRange range, SourcePosition open, SourcePosition close, size_t count)
            : Expression(Kind, range), m_open(open), m_close(close), m_count(count) {}

        // TrailObjects support.
        size_t GetTrailCount(OverloadToken<Expression*>) const { return m_count; }
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return (m_count == 0) ? 0 : m_count - 1; }

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @param index
         * @return a @c SourcePosition representing where ',' positioned.
         */
        SourcePosition GetCommaPositionAt(size_t index) const { return At<SourcePosition>(index); }

        /**
         * @brief Get a position of '['.
         * @return a @c SourcePosition representing where '[' positioned.
         */
        SourcePosition GetOpenSquarePosition() const { return m_open; }

        /**
         * @brief Get a position of ']'.
         * @return a @c SourcePosition representing where ']' positioned.
         */
        SourcePosition GetCloseSquarePosition() const { return m_close; }

        static ListExpression*
        Create(Context& context, SourcePosition open, const std::vector<Expression*>& items,
               const std::vector<SourcePosition>& commas, SourcePosition close);
    }; // end class ListExpression

    /**
     * @brief
     */
    class KeyValuePair final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::KeyValue;

    private:
        Expression* m_key;
        SourcePosition m_colon;
        Expression* m_value;

        KeyValuePair(SourceRange range, Expression* key, SourcePosition colon, Expression* value)
            : Expression(Kind, range), m_key(key), m_colon(colon), m_value(value) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @return
         */
        Expression* GetKey() const { return m_key; }

        /**
         * @brief Get a position of ':'.
         * @return a @c SourcePosition representing where ':' positioned.
         */
        SourcePosition GetColonPosition() const { return m_colon; }

        /**
         * @brief
         * @return
         */
        Expression* GetValue() const { return m_value; }

        static KeyValuePair* Create(Context& context, Expression* key, SourcePosition colon, Expression* value);
    }; // end class KeyValuePair

    /**
     * @brief Represents map.
     */
    class MapExpression final : public Expression, TrailObjects<MapExpression, Expression*, SourcePosition> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = ExpressionKind::Map;

    private:
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        MapExpression(SourceRange range, SourcePosition open, SourcePosition close, size_t count)
            : Expression(Kind, range), m_open(open), m_close(close), m_count(count) {}

        // TrailObjects support.
        size_t GetTrailCount(OverloadToken<Expression*>) const { return m_count; }
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return m_count - 1; }

        ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @param index
         * @return
         */
        SourcePosition GetCommaPositionAt(size_t index) const { return At<SourcePosition>(index); }

        /**
         * @brief Get a position of '{'.
         * @return a @c SourcePosition representing where '{' positioned.
         */
        SourcePosition GetOpenBracePosition() const { return m_open; }

        /**
         * @brief Get a position of '}'.
         * @return a @c SourcePosition representing where '}' positioned.
         */
        SourcePosition GetCloseBracePosition() const { return m_close; }

        static MapExpression*
        Create(Context& context, SourcePosition open, const std::vector<Expression*>& items,
               const std::vector<SourcePosition>& commas, SourcePosition close);
    }; // end class MapExpression

    /**
     * @brief Represents closure.
     */
    class ClosureExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Closure;

    private:
        SourcePosition m_arrow;
        Parameters* m_params;
        ASTNode* m_body;

        ClosureExpression(SourceRange range, SourcePosition arrow, Parameters* params, ASTNode* body)
            : Expression(Kind, range), m_arrow(arrow), m_params(params), m_body(body) {}

        ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of '=>'.
         * @return a @c SourcePosition representing where '=>' positioned.
         */
        SourcePosition GetArrowPosition() const { return m_arrow; }

        /**
         * @brief Get parameters of the closure.
         * @return @c Parameters representing parameters of the closure.
         */
        Parameters* GetParameters() const { return m_params; }

        /**
         * @brief Get body of the closure
         * @return a @c Statement representing body of the closure.
         */
        ASTNode* GetBody() const { return m_body; }

        static ClosureExpression* Create(Context& context, Parameters* params, SourcePosition arrow, ASTNode* body);
    }; // end class ClosureExpression

    /**
     * @brief
     */
    enum class LiteralType {
        None,
        Variable,
        Self,
        Super,
        Integer,
        Float,
        Boolean,
        String
    }; // end class LiteralType

    /**
     * @brief Represents literal.
     */
    class LiteralExpression final : public Expression, TrailObjects<LiteralExpression, Expression*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = ExpressionKind::Literal;

        using ValueUnion = std::variant<int64_t, double, bool, std::string>;

    private:
        LiteralType m_type;
        ValueUnion m_value;
        size_t m_count;

        LiteralExpression(SourceRange range, LiteralType type, ValueUnion value, size_t count = 0)
            : Expression(Kind, range), m_type(type), m_value(std::move(value)), m_count(count) {}

        size_t GetTrailCount(OverloadToken<Expression*>) const { return m_count; } // TrailObjects support.

        ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @return
         */
        LiteralType GetLiteralType() const { return m_type; }

        /**
         * @brief
         * @return
         */
        int64_t AsInteger() const { return std::get<int64_t>(m_value); }

        /**
         * @brief
         * @return
         */
        double AsFloat() const { return std::get<double>(m_value); }

        /**
         * @brief
         * @return
         */
        bool AsBoolean() const { return std::get<bool>(m_value); }

        /**
         * @brief
         * @return
         */
        std::string_view AsString() const { return std::get<std::string>(m_value); }

        /**
         * @brief
         * @return
         */
        bool HasInterpolations() const {
            assert((m_type == LiteralType::String) && "literal is not a string.");
            return m_count > 0;
        }

        // GetRawString() const;

        /**
         * @brief
         * @return
         */
        ValueUnion GetRawValue() const { return m_value; }

        static LiteralExpression* CreateVariable(Context& context, const Identifier& id);

        static LiteralExpression* CreateInteger(Context& context, SourceRange range, int64_t value);

        static LiteralExpression* CreateFloat(Context& context, SourceRange range, double value);

        static LiteralExpression* CreateBoolean(Context& context, SourceRange range, bool value);

        static LiteralExpression*
        CreateString(Context& context, SourceRange range, std::string value, const std::vector<Expression*>& exprs);

        static LiteralExpression* CreateNone(Context& context, SourceRange range);

        static LiteralExpression* CreateSelf(Context& context, SourceRange range);

        static LiteralExpression* CreateSuper(Context& context, SourceRange range);
    }; // end class LiteralExpression
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_EXPRESSIONS_H