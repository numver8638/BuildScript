/*
 * Statements.h
 * - Represents statement in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_STATEMENTS_H
#define BUILDSCRIPT_COMPILER_AST_STATEMENTS_H

#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Utils/TrailObjects.h>

namespace BuildScript {
    class Context;

    enum class StatementKind {
        Invalid,

        Block,
        Arrow,

        If,
        Match,
        Labeled,
        For,
        While,
        With,
        Try,
        Except,
        Finally,
        Break,
        Continue,
        Return,
        Assert,
        Pass,
        Assign,

        Expression
    }; // end enum StatementKind

    /**
     * @brief Represents invalid statement.
     */
    class InvalidStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Invalid;

    private:
        explicit InvalidStatement(SourceRange range)
            : Statement(Kind, range) {}

    public:
        static InvalidStatement* Create(Context& context, SourceRange range);
    }; // end class InvalidStatement

    /**
     * @brief Represent statements surrounded with braces.
     */
    class BlockStatement final : public Statement, TrailObjects<BlockStatement, ASTNode*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = StatementKind::Block;

    private:
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        BlockStatement(SourceRange range, SourcePosition open, SourcePosition close, size_t count)
            : Statement(Kind, range), m_open(open), m_close(close), m_count(count) {}

        size_t GetTrailCount(OverloadToken<ASTNode*>) const { return m_count; } // TrailObjects support.

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
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

        static BlockStatement*
        Create(Context& context, SourcePosition open, const std::vector<ASTNode*>& nodes, SourcePosition close);
    }; // end class BlockStatement

    /**
     * @brief Represent
     */
    class ArrowStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Arrow;

    private:
        SourcePosition m_arrow;
        Expression* m_expr;

        ArrowStatement(SourceRange range, SourcePosition arrow, Expression* expr)
            : Statement(Kind, range), m_arrow(arrow), m_expr(expr) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of '=>'.
         * @return a @c SourcePosition representing where '=>' positioned.
         */
        SourcePosition GetArrowPosition() const { return m_arrow; }

        /**
         * @brief
         * @return
         */
        const Expression* GetExpression() const { return m_expr; }

        static ArrowStatement* Create(Context& context, SourcePosition arrow, Expression* expr);
    }; // end class ArrowStatement

    /**
     * @brief Represents if statement.
     */
    class IfStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::If;

    private:
        SourcePosition m_if;
        Expression* m_condition;
        Statement* m_ifBody;
        SourcePosition m_else;
        Statement* m_elseBody;

        IfStatement(SourceRange range, SourcePosition _if, Expression* condition, Statement* ifBody,
                    SourcePosition _else, Statement* elseBody)
            : Statement(Kind, range), m_if(_if), m_condition(condition), m_ifBody(ifBody), m_else(_else),
              m_elseBody(elseBody) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'if' keyword.
         * @return a @c SourcePosition representing where 'if' keyword positioned.
         */
        SourcePosition GetIfPosition() const { return m_if; }

        /**
         * @brief
         * @return
         */
        const Expression* GetCondition() const { return m_condition; }

        /**
         * @brief
         * @return
         */
        const Statement* GetIfBody() const { return m_ifBody; }

        /**
         * @brief Get a position of 'else' keyword.
         * @return a @c SourcePosition representing where 'else' keyword positioned.
         * @note Return value may be empty if the statement does not have else statement.
         */
        SourcePosition GetElsePosition() const { return m_else; }

        /**
         * @brief
         * @return
         */
        const Statement* GetElseBody() const { return m_elseBody; }

        static IfStatement*
        Create(Context& context, SourcePosition _if, Expression* condition, Statement* ifBody, SourcePosition _else,
               Statement* elseBody);
    }; // end class IfStatement

    /**
     * @brief Represents match statement.
     */
    class MatchStatement final : public Statement, TrailObjects<MatchStatement, Statement*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = StatementKind::Match;

    private:
        SourcePosition m_match;
        Expression* m_condition;
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        MatchStatement(SourceRange range, SourcePosition match, Expression* condition, SourcePosition open,
                       SourcePosition close, size_t count)
            : Statement(Kind, range), m_match(match), m_condition(condition), m_open(open), m_close(close),
              m_count(count) {}

        size_t GetTrailCount(OverloadToken<Statement*>) const { return m_count; } // TrailObjects support.

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'match' keyword.
         * @return a @c SourcePosition representing where 'match' keyword positioned.
         */
        SourcePosition GetMatchPosition() const { return m_match; }

        /**
         * @brief
         * @return
         */
        const Expression* GetCondition() const { return m_condition; }

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

        static MatchStatement*
        Create(Context& context, SourcePosition match, Expression* condition, SourcePosition open,
               const std::vector<Statement*>& nodes, SourcePosition close);
    }; // end class MatchStatement

    /**
     * @brief Represents label.
     */
    class Label final : public ASTNode {
    private:
        SourcePosition m_pos;
        Expression* m_value;
        SourcePosition m_colon;
        bool m_isDefault;

        Label(SourceRange range, SourcePosition pos, Expression* value, SourcePosition colon, bool isDefault)
            : ASTNode(ASTKind::Label, range), m_value(value), m_pos(pos), m_colon(colon), m_isDefault(isDefault) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support

    public:
        /**
         * @brief
         * @return
         */
        bool IsDefault() const { return m_isDefault; }

        /**
         * @brief Get a position of 'default' keyword.
         * @return a @c SourcePosition representing where 'default' keyword positioned.
         * @note Return value may be empty if label is default label.
         */
        SourcePosition GetDefaultPosition() const { return m_isDefault ? m_pos : SourcePosition::Empty(); }

        /**
         * @brief Get a position of 'case' keyword.
         * @return a @c SourcePosition representing where 'case' keyword positioned.
         * @note Return value may be empty if label is case label.
         */
        SourcePosition GetCasePosition() const { return m_isDefault ? SourcePosition::Empty() : m_pos; }

        /**
         * @brief
         * @return
         */
        const Expression* GetCaseValue() const { return m_value; }

        /**
         * @brief Get a position of ':'.
         * @return a @c SourcePosition representing where ':' positioned.
         */
        SourcePosition GetColonPosition() const { return m_colon; }

        static Label*
        Create(Context& context, SourcePosition _case, SourcePosition _default, Expression* expr, SourcePosition colon);
    }; // end class Label

    inline const Label* ASTNode::AsLabel() const {
        return IsLabel() ? static_cast<const Label*>(this) : nullptr; // NOLINT
    }

    /**
     * @brief Represents labels and statements.
     */
    class LabeledStatement final : public Statement, TrailObjects<LabeledStatement, Label*, ASTNode*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = StatementKind::Labeled;

    private:
        size_t m_labelCount;
        size_t m_stmtCount;

        LabeledStatement(SourceRange range, size_t labelCount, size_t stmtCount)
            : Statement(Kind, range), m_labelCount(labelCount), m_stmtCount(stmtCount) {}

        size_t GetTrailCount(OverloadToken<Label*>) const { return m_labelCount; } // TrailObjects support.
        size_t GetTrailCount(OverloadToken<ASTNode*>) const { return m_stmtCount; } // TrailObjects support.

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief
         * @return
         */
        ASTIterator GetLabels() const { return ASTIterator(this); }

        /**
         * @brief
         * @return
         */
        ASTIterator GetStatements() const { return ASTIterator(this); }

        static LabeledStatement*
        Create(Context& context, const std::vector<Label*>& labels, const std::vector<ASTNode*>& nodes);
    }; // end class LabeledStatement

    /**
     * @brief Represents for statement.
     */
    class ForStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::For;

    private:
        SourcePosition m_for;
        Identifier m_param;
        SourcePosition m_in;
        Expression* m_expr;
        Statement* m_body;

        ForStatement(SourceRange range, SourcePosition _for, Identifier param, SourcePosition _in, Expression* expr,
                     Statement* body)
            : Statement(Kind, range), m_for(_for), m_param(std::move(param)), m_in(_in), m_expr(expr), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'for' keyword.
         * @return a @c SourcePosition representing where 'for' keyword positioned.
         */
        SourcePosition GetForPosition() const { return m_for; }

        const Identifier& GetParameterName() const { return m_param; }

        /**
         * @brief Get a position of 'in' keyword.
         * @return a @c SourcePosition representing where 'in' keyword positioned.
         */
        SourcePosition GetInPosition() const { return m_in; }

        /**
         * @brief
         * @return
         */
        const Expression* GetExpression() const { return m_expr; }

        /**
         * @brief
         * @return
         */
        const Statement* GetBody() const { return m_body; }

        static ForStatement*
        Create(Context& context, SourcePosition _for, Identifier param, SourcePosition _in, Expression* expr,
               Statement* body);
    }; // end class ForStatement

    /**
     * @brief Represents while statement.
     */
    class WhileStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::While;

    private:
        SourcePosition m_while;
        Expression* m_condition;
        Statement* m_body;

        WhileStatement(SourceRange range, SourcePosition _while, Expression* cond, Statement* body)
            : Statement(Kind, range), m_while(_while), m_condition(cond), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'while' keyword.
         * @return a @c SourcePosition representing where 'while' keyword positioned.
         */
        SourcePosition GetWhilePosition() const { return m_while; }

        /**
         * @brief
         * @return
         */
        const Expression* GetCondition() const { return m_condition; }

        /**
         * @brief
         * @return
         */
        const Statement* GetBody() const { return m_body; }

        static WhileStatement* Create(Context& context, SourcePosition _while, Expression* condition, Statement* body);
    }; // end class WhileStatement

    /**
     * @brief Represents with statement.
     */
    class WithStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::With;

    private:
        SourcePosition m_with;
        Expression* m_expr;
        SourcePosition m_as;
        Identifier m_capture;
        Statement* m_body;

        WithStatement(SourceRange range, SourcePosition with, Expression* expr, SourcePosition as, Identifier capture,
                      Statement* body)
            : Statement(Kind, range), m_with(with), m_expr(expr), m_as(as), m_capture(std::move(capture)), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'with' keyword.
         * @return a @c SourcePosition representing where 'with' keyword positioned.
         */
        SourcePosition GetWithPosition() const { return m_with; }

        /**
         * @brief
         * @return
         */
        const Expression* GetExpression() const { return m_expr; }

        /**
         * @brief
         * @return
         */
        bool HasCapture() const { return (bool)m_as; }

        /**
         * @brief Get a position of 'as' keyword.
         * @return a @c SourcePosition representing where 'as' keyword positioned.
         * @note Return value may be empty if the statement does not have captured variable.
         */
        SourcePosition GetAsPosition() const { return m_as; }

        /**
         * @brief
         * @return
         */
        const Identifier& GetCaptureName() const { return m_capture; }

        /**
         * @brief
         * @return
         */
        const Statement* GetBody() const { return m_body; }

        static WithStatement* Create(Context& context, SourcePosition with, Expression* expr, SourcePosition as, Identifier capture, Statement* body);
    }; // end class WithStatement

    /**
     * @brief Represents try statement.
     */
    class TryStatement final : public Statement, TrailObjects<TryStatement, Statement*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = StatementKind::Try;

    private:
        SourcePosition m_try;
        size_t m_count;

        TryStatement(SourceRange range, SourcePosition tryPos, size_t count)
            : Statement(Kind, range), m_try(tryPos), m_count(count) {}

        size_t GetTrailCount(OverloadToken<Statement*>) const { return m_count; }

        const ASTNode* GetChild(size_t index) const override;

    public:
        /**
         * @brief Get a position of 'try' keyword.
         * @return a @c SourcePosition representing where 'try' keyword positioned.
         */
        SourcePosition GetTryPosition() const { return m_try; }

        /**
         * @brief
         * @return
         */
        const Statement* GetBody() const { return At<Statement*>(0); }

        /**
         * @brief
         * @return
         */
        ASTIterator GetHandlers() const { return ASTIterator(this); }

        static TryStatement* Create(Context& context, SourcePosition tryPos, const std::vector<Statement*>& handlers);
    }; // end class TryStatement

    /**
     * @brief Represents except statement trailed by try statement.
     */
    class ExceptStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Except;

    private:
        SourcePosition m_except;
        Identifier m_typename;
        SourcePosition m_as;
        Identifier m_capture;
        Statement* m_body;

        ExceptStatement(SourceRange range, SourcePosition exceptPos, Identifier _typename, SourcePosition as,
                        Identifier capture, Statement* body)
            : Statement(Kind, range), m_except(exceptPos), m_typename(std::move(_typename)), m_as(as),
              m_capture(std::move(capture)), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'except' keyword.
         * @return a @c SourcePosition representing where 'except' keyword positioned.
         */
        SourcePosition GetExceptPosition() const { return m_except; }

        /**
         * @brief
         * @return
         */
        const Identifier& GetTypename() const { return m_typename; }

        /**
         * @brief
         * @return
         */
        bool HasCapture() const { return (bool)m_as; }

        /**
         * @brief Get a position of 'as' keyword.
         * @return a @c SourcePosition representing where 'as' keyword positioned.
         * @note Return value may be empty if the statement does not have captured variable.
         */
        SourcePosition GetAsPosition() const { return m_as; }

        /**
         * @brief
         * @return
         */
        const Identifier& GetCaptureName() const { return m_capture; }

        /**
         * @brief
         * @return
         */
        const Statement* GetBody() const { return m_body; }

        static ExceptStatement*
        Create(Context& context, SourcePosition exceptPos, Identifier _typename, SourcePosition as, Identifier capture,
               Statement* body);
    }; // end class ExceptStatement

    /**
     * @brief Represents finally statement trailed by try statement.
     */
    class FinallyStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Finally;

    private:
        SourcePosition m_finally;
        Statement* m_body;

        FinallyStatement(SourceRange range, SourcePosition finallyPos, Statement* body)
            : Statement(Kind, range), m_finally(finallyPos), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'finally' keyword.
         * @return a @c SourcePosition representing where 'finally' keyword positioned.
         */
        SourcePosition GetFinallyPosition() const { return m_finally; }

        /**
         * @brief
         * @return
         */
        const Statement* GetBody() const { return m_body; }

        static FinallyStatement* Create(Context& context, SourcePosition finallyPos, Statement* body);
    }; // end class FinallyStatement

    /**
     * @brief Represents break statement.
     */
    class BreakStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Break;

    private:
        SourcePosition m_break;
        SourcePosition m_if;
        Expression* m_condition;

        BreakStatement(SourceRange range, SourcePosition _break, SourcePosition _if, Expression* condition)
            : Statement(Kind, range), m_break(_break), m_if(_if), m_condition(condition) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'break' keyword.
         * @return a @c SourcePosition representing where 'break' keyword positioned.
         */
        SourcePosition GetBreakPosition() const { return m_break; }

        /**
         * @brief
         * @return
         */
        bool IsConditional() const { return (bool)m_if; }

        /**
         * @brief Get a position of 'if' keyword.
         * @return a @c SourcePosition representing where 'if' keyword positioned.
         * @note Return value may be empty if the statement is not conditional.
         */
        SourcePosition GetIfPosition() const { return m_if; }

        /**
         * @brief
         * @return
         */
        const Expression* GetCondition() const { return m_condition; }

        static BreakStatement* Create(Context& context, SourceRange _break, SourcePosition _if, Expression* condition);
    }; // end class BreakStatement

    /**
     * @brief Represents continue statement.
     */
    class ContinueStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Continue;

    private:
        SourcePosition m_continue;
        SourcePosition m_if;
        Expression* m_condition;

        ContinueStatement(SourceRange range, SourcePosition _continue, SourcePosition _if, Expression* condition)
            : Statement(Kind, range), m_continue(_continue), m_if(_if), m_condition(condition) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'continue' keyword.
         * @return a @c SourcePosition representing where 'continue' keyword positioned.
         */
        SourcePosition GetContinuePosition() const { return m_continue; }

        /**
         * @brief
         * @return
         */
        bool IsConditional() const { return (bool)m_if; }

        /**
         * @brief Get a position of 'if' keyword.
         * @return a @c SourcePosition representing where 'if' keyword positioned.
         * @note Return value may be empty if the statement is not conditional.
         */
        SourcePosition GetIfPosition() const { return m_if; }

        /**
         * @brief
         * @return
         */
        const Expression* GetCondition() const { return m_condition; }

        static ContinueStatement*
        Create(Context& context, SourceRange _continue, SourcePosition _if, Expression* condition);
    }; // end class ContinueStatement

    /**
     * @brief Represents return statement.
     */
    class ReturnStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Return;

    private:
        SourcePosition m_return;
        Expression* m_retval;

        ReturnStatement(SourceRange range, SourcePosition _return, Expression* retval)
            : Statement(Kind, range), m_return(_return), m_retval(retval) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'return' keyword.
         * @return a @c SourcePosition representing where 'return' keyword positioned.
         */
        SourcePosition GetReturnPosition() const { return m_return; }

        /**
         * @brief
         * @return
         */
        bool HasReturnValue() const { return m_retval != nullptr; }

        /**
         * @brief
         * @return
         */
        const Expression* GetReturnValue() const { return m_retval; }

        static ReturnStatement* Create(Context& context, SourceRange _return, Expression* returnValue);
    }; // end class ReturnStatement

    /**
     * @brief Represents assert statement.
     */
    class AssertStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Assert;

    private:
        SourcePosition m_assert;
        Expression* m_condition;
        SourcePosition m_colon;
        Expression* m_message;

        AssertStatement(SourceRange range, SourcePosition _assert, Expression* condition, SourcePosition colon,
                        Expression* message)
            : Statement(Kind, range), m_assert(_assert), m_condition(condition), m_colon(colon), m_message(message) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support

    public:
        /**
         * @brief Get a position of 'assert' keyword.
         * @return a @c SourcePosition representing where 'assert' keyword positioned.
         */
        SourcePosition GetAssertPosition() const { return m_assert; }

        /**
         * @brief
         * @return
         */
        const Expression* GetCondition() const { return m_condition; }

        /**
         * @brief Get a position of ':'.
         * @return a @c SourcePosition representing where ':' positioned.
         */
        SourcePosition GetColonPosition() const { return m_colon; }

        /**
         * @brief
         * @return
         */
        const Expression* GetMessage() const { return m_message; }

        static AssertStatement*
        Create(Context& context, SourcePosition _assert, Expression* condition, SourcePosition colon,
               Expression* message);
    }; // end class AssertStatement

    /**
     * @brief Represents pass statement.
     */
    class PassStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Pass;

    private:
        SourcePosition m_pass;

        PassStatement(SourceRange range, SourcePosition pass)
            : Statement(Kind, range), m_pass(pass) {}

    public:
        /**
         * @brief Get a position of 'pass' keyword.
         * @return a @c SourcePosition representing where 'pass' keyword positioned.
         */
        SourcePosition GetPassPosition() const { return m_pass; }

        static PassStatement* Create(Context& context, SourceRange pass);
    }; // end class PassStatement

    /**
     * @brief
     */
    enum class AssignOp {
        Assign,
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        BitAnd,
        BitOr,
        BitXor,
        LeftShift,
        RightShift
    }; // end enum AssignOp

    /**
     * @brief Represent assignment statement.
     */
    class AssignStatement final : public Statement {
    public:
        static constexpr auto Kind = StatementKind::Assign;

    private:
        Expression* m_target;
        AssignOp m_op;
        SourcePosition m_pos;
        Expression* m_value;

        AssignStatement(SourceRange range, Expression* target, AssignOp op, SourcePosition pos, Expression* value)
            : Statement(Kind, range), m_target(target), m_op(op), m_pos(pos), m_value(value) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support

    public:
        /**
         * @brief
         * @return
         */
        AssignOp GetOp() const { return m_op; }

        /**
         * @brief Get a position of the operator.
         * @return a @c SourcePosition representing where the operator positioned.
         */
        SourcePosition GetOpPosition() const { return m_pos; }

        /**
         * @brief
         * @return
         */
        Expression* GetTarget() const { return m_target; }

        /**
         * @brief
         * @return
         */
        Expression* GetValue() const { return m_value; }

        static AssignStatement*
        Create(Context& context, Expression* target, AssignOp op, SourcePosition pos, Expression* value);
    }; // end class AssignStatement
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_STATEMENTS_H