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
    class Context; // Defined in <BuildScript/Compiler/Context.h>

    enum class ExpressionKind {
        Invalid,

        Pass,       // Used for empty closure.

        Ternary,

        // Binary expressions
        Binary,
        TypeTest,
        ContainmentTest,

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
        SourceRange m_range;

        explicit InvalidExpression(SourceRange range)
            : Expression(Kind), m_range(range) {}

    public:
        /**
         * @brief Get a range of erroneous expression.
         * @return the range of the expression.
         */
        SourceRange GetRange() const { return m_range; }

        static InvalidExpression* Create(Context& context, SourceRange range);
    }; // end class InvalidExpression

    /**
     * @brief Represents pass keyword in closure.
     */
    class PassExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::Pass;

    private:
        SourcePosition m_pass;

        explicit PassExpression(SourcePosition pass)
            : Expression(Kind), m_pass(pass) {}

    public:
        SourcePosition GetPassPosition() const { return m_pass; }

        static PassExpression* Create(Context& context, SourcePosition pass);
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

        TernaryExpression(Expression* valueT, SourcePosition _if, Expression* cond,
                          SourcePosition _else, Expression* valueF)
            : Expression(Kind), m_valueT(valueT), m_if(_if), m_cond(cond), m_else(_else), m_valueF(valueF) {}

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
        SourcePosition m_pos;
        Expression* m_right;

        BinaryExpression(Expression* left, BinaryOp op, SourcePosition pos, Expression* right)
            : Expression(Kind), m_left(left), m_op(op), m_pos(pos), m_right(right) {}

    public:
        /**
         * @brief Get left hand side expression.
         * @return an @c Expression representing left hand side expression.
         */
        const Expression* GetLeft() const { return m_left; }

        /**
         * @brief Get an operation of the expression.
         * @return
         */
        BinaryOp GetOp() const { return m_op; }

        /**
         * @brief Get a position of the operator.
         * @return a @c SourcePosition representing where first operator positioned.
         */
        SourcePosition GetOpPosition() const { return m_pos; }

        /**
         * @brief Get right hand side expression.
         * @return an @c Expression representing right hand side expression.
         */
        const Expression* GetRight() const { return m_right; }

        static BinaryExpression*
        Create(Context& context, Expression* left, BinaryOp op, SourcePosition pos, Expression* right);
    }; // end class BinaryExpression

    /**
     * @brief Represents type testing expression.
     */
    class TypeTestExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::TypeTest;

    private:
        Expression* m_target;
        std::array<SourcePosition, 2> m_pos;
        bool m_negative;
        Identifier m_type;

        TypeTestExpression(Expression* target, std::array<SourcePosition, 2> pos, bool negative, Identifier type)
            : Expression(Kind), m_target(target), m_pos(pos), m_negative(negative), m_type(std::move(type)) {}

    public:
        /**
         * @brief Get the target of the test.
         * @return an @c Expression representing the target.
         */
        const Expression* GetTarget() const { return m_target; }

        /**
         * @brief Check the expression is negative form.
         * @return @c true if the expression is negative otherwise @c false.
         */
        bool IsNegative() const { return m_negative; }

        /**
         * @brief Get a position of 'is' keyword.
         * @return a @c SourcePosition representing where 'is' keyword positioned.
         */
        SourcePosition GetIsPosition() const { return m_pos[0]; }

        /**
         * @brief Get a position of 'not' keyword.
         * @return a @c SourcePosition representing where 'not' keyword positioned.
         * @note Return value maybe empty if the expression is not negate expression.
         */
        SourcePosition GetNotPosition() const { return m_pos[1]; }

        /**
         * @brief Get name of the type.
         * @return an @c Identifier representing the typename.
         */
        const Identifier& GetTypename() const { return m_type; }

        static TypeTestExpression*
        Create(Context& context, Expression* left, std::array<SourcePosition, 2> pos, bool negative, Identifier type);
    }; // end class TypeTestExpression

    /**
     * @brief Represents expression that tests the target contains the value.
     */
    class ContainmentTestExpression final : public Expression {
    public:
        static constexpr auto Kind = ExpressionKind::ContainmentTest;

    private:
        Expression* m_value;
        std::array<SourcePosition, 2> m_pos;
        bool m_negative;
        Expression* m_target;

        ContainmentTestExpression(Expression* value, std::array<SourcePosition, 2> pos, bool negative,
                                  Expression* target)
            : Expression(Kind), m_value(value), m_pos(pos), m_negative(negative), m_target(target) {}

    public:
        /**
         * @brief Get the value of the test.
         * @return an @c Expression representing the value.
         */
        const Expression* GetValue() const { return m_value; }

        /**
         * @brief Check the expression is negative form.
         * @return @c true if the expression is negative otherwise @c false.
         */
        bool IsNegative() const { return m_negative; }

        /**
         * @brief Get a position of 'in' keyword.
         * @return a @c SourcePosition representing where 'in' keyword positioned.
         */
        SourcePosition GetInPosition() const { return m_negative ? m_pos[1] : m_pos[0]; }

        /**
         * @brief Get a position of 'not' keyword.
         * @return a @c SourcePosition representing where 'not' keyword positioned.
         * @note Return value maybe empty if the expression is not negate expression.
         */
        SourcePosition GetNotPosition() const { return m_negative ? m_pos[0] : SourcePosition::Empty(); }

        /**
         * @brief Get the target of the test.
         * @return an @c Expression representing the target.
         */
        const Expression* GetTarget() const { return m_target; }

        static ContainmentTestExpression* Create(Context& context, Expression* value, std::array<SourcePosition, 2> pos,
                                                 bool negative, Expression* target);
    }; // end class ContainmentTestExpression

    /**
     * @brief Represents unary operator.
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

        UnaryExpression(UnaryOp op, SourcePosition pos, Expression* expr)
            : Expression(Kind), m_op(op), m_pos(pos), m_expr(expr) {}

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

        DefinedExpression(SourcePosition defined, Identifier id, SourcePosition in,
                          Expression* target)
            : Expression(Kind), m_defined(defined), m_id(std::move(id)), m_in(in), m_target(target) {}

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
         * @brief
         * @return
         */
        bool HasTarget() const { return (bool)m_in; }

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

        RaiseExpression(SourcePosition raise, Expression* target)
            : Expression(Kind), m_raise(raise), m_target(target) {}

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

        MemberAccessExpression(Expression* target, SourcePosition dot, Identifier name)
            : Expression(Kind), m_target(target), m_dot(dot), m_name(std::move(name)) {}

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
        Expression* m_target;
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        InvocationExpression(Expression* target, SourcePosition open, SourcePosition close, size_t count)
            : Expression(Kind), m_target(target), m_open(open), m_close(close), m_count(count) {}

        size_t GetTrailCount(OverloadToken<Expression*>) const { return m_count; } // TrailObjects support.
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return GetArgumentCount(); } // TrailObjects support.

    public:
        /**
         * @brief Get an expression that is target of invocation.
         * @return
         */
        Expression* GetTarget() const { return m_target; }

        /**
         * @brief Get a position of '('.
         * @return a @c SourcePosition representing where '(' positioned.
         */
        SourcePosition GetOpenParenPosition() const { return m_open; }

        /**
         * @brief
         * @return
         */
        TrailIterator<Expression*> GetArguments() const { return GetTrailObjects<Expression*>(); }

        /**
         * @brief Get count of arguments.
         * @return a count of arguments.
         */
        size_t GetArgumentCount() const { return (m_count > 1) ? m_count - 1 : 0; }

        /**
         * @brief
         * @return
         */
        TrailIterator<SourcePosition> GetCommaPositions() const { return GetTrailObjects<SourcePosition>(); }

        /**
         * @brief Get a position of ')'.
         * @return a @c SourcePosition representing where ')' positioned.
         */
        SourcePosition GetCloseParenPosition() const { return m_close; }

        static InvocationExpression*
        Create(Context& context, Expression* target, SourcePosition open, const std::vector<Expression*>& args,
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

        SubscriptExpression(Expression* target, SourcePosition open, Expression* index, SourcePosition close)
            : Expression(Kind), m_target(target), m_open(open), m_index(index), m_close(close) {}

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

        ParenthesizedExpression(SourcePosition open, SourcePosition close, Expression* expr)
            : Expression(Kind), m_open(open), m_close(close), m_expr(expr) {}

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

        ListExpression(SourcePosition open, SourcePosition close, size_t count)
            : Expression(Kind), m_open(open), m_close(close), m_count(count) {}

        // TrailObjects support.
        size_t GetTrailCount(OverloadToken<Expression*>) const { return m_count; }
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return (m_count == 0) ? 0 : m_count - 1; }

    public:
        /**
         * @brief
         * @return
         */
        TrailIterator<Expression*> GetItems() const { return GetTrailObjects<Expression*>(); }

        /**
         * @brief
         * @return
         */
        TrailIterator<SourcePosition> GetCommas() const { return GetTrailObjects<SourcePosition>(); }

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
     * @brief Represents key and value in map.
     */
    using KeyValuePair = std::tuple<Expression*, SourcePosition, Expression*>;

    /**
     * @brief Represents map.
     */
    class MapExpression final : public Expression, TrailObjects<MapExpression, KeyValuePair, SourcePosition> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = ExpressionKind::Map;

    private:
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        MapExpression(SourcePosition open, SourcePosition close, size_t count)
            : Expression(Kind), m_open(open), m_close(close), m_count(count) {}

        // TrailObjects support.
        size_t GetTrailCount(OverloadToken<KeyValuePair>) const { return m_count; }
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return m_count - 1; }

    public:
        /**
         * @brief
         * @return
         */
        TrailIterator<KeyValuePair> GetItems() const { return GetTrailObjects<KeyValuePair>(); }

        /**
         * @brief
         * @return
         */
        TrailIterator<SourcePosition> GetCommas() const { return GetTrailObjects<SourcePosition>(); }

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
        Create(Context& context, SourcePosition open, const std::vector<KeyValuePair>& items,
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

        ClosureExpression(SourcePosition arrow, Parameters* params, ASTNode* body)
            : Expression(Kind), m_arrow(arrow), m_params(params), m_body(body) {}

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
        None,       //!<
        Variable,   //!<
        Self,       //!<
        Super,      //!<
        Integer,    //!<
        Float,      //!<
        Boolean,    //!<
        String      //!<
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
        SourceRange m_range;
        LiteralType m_type;
        ValueUnion m_value;
        size_t m_count;

        LiteralExpression(SourceRange range, LiteralType type, ValueUnion value, size_t count = 0)
            : Expression(Kind), m_range(range), m_type(type), m_value(std::move(value)), m_count(count) {}

        size_t GetTrailCount(OverloadToken<Expression*>) const { return m_count; } // TrailObjects support.

    public:
        /**
         * @brief Get a position of the literal.
         * @return
         */
        SourcePosition GetPosition() const { return m_range.Begin; }

        /**
         * @brief Get a range of the literal.
         * @return
         */
        SourceRange GetRange() const { return m_range; }

        /**
         * @brief Get a type of the literal.
         * @return
         */
        LiteralType GetLiteralType() const { return m_type; }

        /**
         * @brief Get value of the literal as integer.
         * @return an integer value.
         * @warning This member function may throw if the literal is not an integer.
         */
        int64_t AsInteger() const { return std::get<int64_t>(m_value); }

        /**
         * @brief Get value of the literal as float.
         * @return a float value.
         * @warning This member function may throw if the literal is not a float.
         */
        double AsFloat() const { return std::get<double>(m_value); }

        /**
         * @brief Get value of the literal as boolean.
         * @return a boolean value.
         * @warning This member function may throw if the literal is not a boolean.
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

        /**
         * @brief Get count of interpolated expressions.
         * @return
         */
        size_t GetInterpolationCount() const { return m_count; }

        /**
         * @brief
         * @return
         */
        TrailIterator<Expression*> GetInterpolations() const { return GetTrailObjects<Expression*>(); }

        // GetRawString() const;

        /**
         * @brief
         * @return
         * @warning Value may empty
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