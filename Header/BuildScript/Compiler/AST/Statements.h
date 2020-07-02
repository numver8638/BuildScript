/*
 * Statements.h
 * - Represent statement in AST.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_STATEMENTS_H
#define BUILDSCRIPT_COMPILER_AST_STATEMENTS_H

#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/AST/Expressions.h>

namespace BuildScript {
    class ErrorStatement final : public Statement {
    public:
        explicit ErrorStatement(const SourcePosition& pos)
            : Statement(pos) {}

        void Accept(ASTVisitor*) const override;
    }; // end class ErrorStatement

    class BlockStatement final : public Statement {
    private:
        std::vector<Statement*> m_stmts;

    public:
        explicit BlockStatement(std::vector<Statement*>&& stmts)
            : Statement(SourcePosition()), m_stmts(std::move(stmts)) {}

        ~BlockStatement() final {
            for (auto e : m_stmts)
                delete e;
        }

        const std::vector<Statement*>& GetStatements() const { return m_stmts; }

        void Accept(ASTVisitor*) const override;
    }; // end class BlockStatement

    enum class AssignOp {
        Assign,
        InplaceAdd,
        InplaceSub,
        InplaceMul,
        InplaceDiv,
        InplaceMod,
        InplaceBitAnd,
        InplaceBitOr,
        InplaceBitXor,
        InplaceLeftShift,
        InplaceRightShift
    }; // end enum AssignOp

    class AssignmentStatement final : public Statement {
    private:
        Expression* m_target;
        Expression* m_value;
        AssignOp m_op;

    public:
        AssignmentStatement(Expression* target, Expression* value, AssignOp op)
            : Statement(target->GetPosition()), m_target(target), m_value(value), m_op(op) {}

        ~AssignmentStatement() final {
            delete m_target;
            delete m_value;
        }

        const Expression* GetTarget() const { return m_target; }

        const Expression* GetValue() const { return m_value; }

        AssignOp GetOperator() const { return m_op; }

        void Accept(ASTVisitor*) const override;
    }; // end class AssignmentStatement

    class IfStatement final : public Statement {
    private:
        std::map<Expression*, Statement*> m_then;
        Statement* m_else;

    public:
        IfStatement(const SourcePosition& pos, std::map<Expression*, Statement*>&& then, Statement* _else)
            : Statement(pos), m_then(std::move(then)), m_else(_else) {}

        ~IfStatement() final {
            for (auto& e : m_then) {
                delete e.first;
                delete e.second;
            }

            delete m_else;
        }

        const std::map<Expression*, Statement*>& GetThenClauses() const { return m_then; }

        const Statement* GetElseClause() const { return m_else; }

        void Accept(ASTVisitor*) const override;
    }; // end class IfStatement

    class LabeledStatement final : public Statement {
    public:
        using Label = std::tuple<SourcePosition, Expression*>;

    private:
        std::vector<Label> m_labels;
        std::vector<Statement*> m_stmts;

    public:
        LabeledStatement(std::vector<Label>&& labels, std::vector<Statement*>&& stmts)
            : Statement(SourcePosition()), m_labels(std::move(labels)), m_stmts(std::move(stmts)) {}

        ~LabeledStatement() final {
            for (auto& e : m_labels)
                delete std::get<1>(e);

            for (auto e : m_stmts)
                delete e;
        }

        const std::vector<Label>& GetLabels() const { return m_labels; }

        const std::vector<Statement*>& GetStatements() const { return m_stmts; }

        void Accept(ASTVisitor*) const override;
    };

    class MatchStatement final : public Statement {
    private:
        Expression* m_expr;
        std::vector<LabeledStatement*> m_stmts;

    public:
        MatchStatement(const SourcePosition& pos, Expression* expr, std::vector<LabeledStatement*>&& stmts)
            : Statement(pos), m_expr(expr), m_stmts(std::move(stmts)) {}

        ~MatchStatement() final {
            delete m_expr;
            for (auto e : m_stmts)
                delete e;
        }

        const Expression* GetTarget() const { return m_expr; }

        const std::vector<LabeledStatement*>& GetLabeledStatements() const { return m_stmts; }

        void Accept(ASTVisitor*) const override;
    }; // end class MatchStatement

    class ForStatement final : public Statement {
    private:
        std::vector<StringRef> m_params;
        Expression* m_expr;
        Statement* m_body;

    public:
        ForStatement(const SourcePosition& pos, std::vector<StringRef>&& params, Expression* expr, Statement* body)
            : Statement(pos), m_params(std::move(params)), m_expr(expr), m_body(body) {}

        ~ForStatement() final {
            delete m_expr;
            delete m_body;
        }

        const std::vector<StringRef>& GetParameters() const { return m_params; }

        const Expression* GetExpression() const { return m_expr; }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    }; // end class ForStatement

    class WhileStatement final : public Statement {
    private:
        Expression* m_condition;
        Statement* m_body;

    public:
        WhileStatement(const SourcePosition& pos, Expression* condition, Statement* body)
            : Statement(pos), m_condition(condition), m_body(body) {}

        ~WhileStatement() final {
            delete m_condition;
            delete m_body;
        }

        const Expression* GetCondition() const { return m_condition; }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    }; // end class WhileStatement

    class ExceptClause {
    private:
        SourcePosition m_pos;
        StringRef m_typename;
        StringRef m_varname;
        Statement* m_body;

    public:
        ExceptClause(const SourcePosition& pos, const StringRef& tname, const StringRef& vname, Statement* body)
            : m_pos(SourcePosition()), m_typename(tname), m_varname(vname), m_body(body) {}

        ~ExceptClause() {
            delete m_body;
        }

        const StringRef& GetTypeName() const { return m_typename; }

        const StringRef& GetVarName() const { return m_varname; }

        const Statement* GetBody() const { return m_body; }

        const SourcePosition& GetPosition() const { return m_pos; }
    };

    class TryStatement final : public Statement {
    private:
        Statement* m_body;
        std::vector<ExceptClause*> m_excepts;
        Statement* m_finally;

    public:
        TryStatement(const SourcePosition& pos, Statement* body, std::vector<ExceptClause*>&& excepts,
                     Statement* finally)
            : Statement(pos), m_body(body), m_excepts(std::move(excepts)), m_finally(finally) {}

        ~TryStatement() final {
            delete m_body;
            for (auto e : m_excepts)
                delete e;
            delete m_finally;
        }

        const std::vector<ExceptClause*>& GetExceptClauses() const { return m_excepts; }

        const Statement* GetBody() const { return m_body; }

        const Statement* GetFinallyClause() const { return m_finally; }

        void Accept(ASTVisitor*) const override;
    }; // end class TryStatement

    class RaiseStatement final : public Statement {
    private:
        Expression* m_expr;

    public:
        RaiseStatement(const SourcePosition& pos, Expression* expr)
            : Statement(pos), m_expr(expr) {}

        ~RaiseStatement() final {
            delete m_expr;
        }

        const Expression* GetExpression() const { return m_expr; }

        void Accept(ASTVisitor*) const override;
    }; // end class RaiseStatement

    class BreakStatement final : public Statement {
    private:
        Expression* m_condition;

    public:
        BreakStatement(const SourcePosition& pos, Expression* condition)
            : Statement(pos), m_condition(condition) {}

        ~BreakStatement() final {
            delete m_condition;
        }

        const Expression* GetCondition() const { return m_condition; }

        void Accept(ASTVisitor*) const override;
    }; // end class BreakStatement

    class ContinueStatement final : public Statement {
    private:
        Expression* m_condition;

    public:
        ContinueStatement(const SourcePosition& pos, Expression* condition)
            : Statement(pos), m_condition(condition) {}

        ~ContinueStatement() final {
            delete m_condition;
        }

        const Expression* GetCondition() const { return m_condition; }

        void Accept(ASTVisitor*) const override;
    }; // end class ContinueStatement

    class ReturnStatement final : public Statement {
    private:
        Expression* m_retval;

    public:
        ReturnStatement(const SourcePosition& pos, Expression* retval)
            : Statement(pos), m_retval(retval) {}

        ~ReturnStatement() final {
            delete m_retval;
        }

        const Expression* GetExpression() const { return m_retval; }

        void Accept(ASTVisitor*) const override;
    }; // end class ReturnStatement

    class EmptyStatement final : public Statement {
    public:
        explicit EmptyStatement(const SourcePosition& pos)
            : Statement(pos) {}

        void Accept(ASTVisitor*) const override;
    }; // end class EmptyStatement

    class AssertStatement final : public Statement {
    private:
        Expression* m_assertion;
        Expression* m_message;

    public:
        AssertStatement(const SourcePosition& pos, Expression* assertion, Expression* message)
            : Statement(pos), m_assertion(assertion), m_message(message) {}

        ~AssertStatement() final {
            delete m_assertion;
            delete m_message;
        }

        const Expression* GetCondition() const { return m_assertion; }

        const Expression* GetMessage() const { return m_message; }

        void Accept(ASTVisitor*) const override;
    }; // end class AssertStatement

    class VariableDeclaration final : public Statement {
    private:
        StringRef m_name;
        Expression* m_value;

    public:
        VariableDeclaration(const SourcePosition& pos, const StringRef& name, Expression* value)
            : Statement(pos), m_name(name), m_value(value) {}

        ~VariableDeclaration() final {
            delete m_value;
        }

        const StringRef& GetName() const { return m_name; }

        const Expression* GetValue() const { return m_value; }

        void Accept(ASTVisitor*) const override;
    }; // end class VariableDeclaration
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_STATEMENTS_H