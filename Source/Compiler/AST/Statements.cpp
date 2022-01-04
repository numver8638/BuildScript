/*
 * Statements.cpp
 * - Represents statement in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/Statements.h>

#include <BuildScript/Compiler/Context.h>

using namespace BuildScript;

InvalidStatement* InvalidStatement::Create(Context& context, SourceRange range) {
    return new (context.GetAllocator()) InvalidStatement(range);
}

BlockStatement*
BlockStatement::Create(Context& context, SourcePosition open, const std::vector<ASTNode*>& nodes,
                       SourcePosition close) {
    auto trailSize = GetTrailSize(nodes.size());
    auto* node = new (context.GetAllocator(), trailSize) BlockStatement(open, close, nodes.size());

    node->SetTrailObjects(nodes.data(), nodes.size());

    return node;
}

const ASTNode* BlockStatement::GetChild(size_t index) const {
    return (index < m_count) ? At<ASTNode*>(index) : nullptr;
}

ArrowStatement* ArrowStatement::Create(Context& context, SourcePosition arrow, Expression* expr) {
    return new (context.GetAllocator()) ArrowStatement(arrow, expr);
}

const ASTNode* ArrowStatement::GetChild(size_t index) const {
    return (index == 0) ? m_expr : nullptr;
}

IfStatement*
IfStatement::Create(Context& context, SourcePosition _if, Expression* condition, Statement* ifBody,
                    SourcePosition _else, Statement* elseBody) {
    return new (context.GetAllocator()) IfStatement(_if, condition, ifBody, _else, elseBody);
}

const ASTNode* IfStatement::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_condition;
        case 1: return m_ifBody;
        case 2: return m_elseBody;
        default: return nullptr;
    }
}

MatchStatement*
MatchStatement::Create(Context& context, SourcePosition match, Expression* condition, SourcePosition open,
                       const std::vector<Statement*>& nodes, SourcePosition close) {
    auto trailSize = GetTrailSize(nodes.size());
    auto* node = new (context.GetAllocator(), trailSize) MatchStatement(match, condition, open, close, nodes.size());

    node->SetTrailObjects(nodes.data(), nodes.size());

    return node;
}

const ASTNode* MatchStatement::GetChild(size_t index) const {
    if (index == 0) {
        return m_condition;
    }
    else if ((index - 1) < m_count) {
        return At<Statement*>(index - 1);
    }
    else {
        return nullptr;
    }
}

Label*
Label::Create(Context& context, SourcePosition _case, SourcePosition _default, Expression* expr, SourcePosition colon) {
    assert(!(_case && _default) && "cannot be both case label and default label.");

    auto pos = _default ? _default : _case;
    return new (context.GetAllocator()) Label(pos, expr, colon, (bool)_default);
}

const ASTNode* Label::GetChild(size_t index) const {
    return (index == 0) ? m_value : nullptr;
}

LabeledStatement*
LabeledStatement::Create(Context& context, const std::vector<Label*>& labels, const std::vector<ASTNode*>& nodes) {
    assert((!labels.empty() || !nodes.empty()) && "cannot be both labels and statements are empty.");

    auto trailSize = GetTrailSize(labels.size(), nodes.size());
    auto* node = new (context.GetAllocator(), trailSize) LabeledStatement(labels.size(), nodes.size());

    node->SetTrailObjects(labels.data(), labels.size());
    node->SetTrailObjects(nodes.data(), nodes.size());

    return node;
}

const ASTNode* LabeledStatement::GetChild(size_t index) const {
    if (index < m_labelCount) {
        return At<Label*>(index);
    }
    else if (index < (m_labelCount + m_stmtCount)) {
        return At<ASTNode*>(index - m_labelCount);
    }
    else {
        return nullptr;
    }
}

ForStatement*
ForStatement::Create(Context& context, SourcePosition _for, Identifier param, SourcePosition _in, Expression* expr,
                     Statement* body) {
    auto* node = new (context.GetAllocator()) ForStatement(_for, std::move(param), _in, expr, body);

    return node;
}

const ASTNode* ForStatement::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_expr;
        case 1: return m_body;
        default: return nullptr;
    }
}

WhileStatement*
WhileStatement::Create(Context& context, SourcePosition _while, Expression* condition, Statement* body) {
    return new (context.GetAllocator()) WhileStatement(_while, condition, body);
}

const ASTNode* WhileStatement::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_condition;
        case 1: return m_body;
        default: return nullptr;
    }
}

WithStatement*
WithStatement::Create(Context& context, SourcePosition with, Expression* expr, SourcePosition as, Identifier capture,
                      Statement* body) {
    return new (context.GetAllocator()) WithStatement(with, expr, as, std::move(capture), body);
}

const ASTNode* WithStatement::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_expr;
        case 1: return m_body;
        default: return nullptr;
    }
}

TryStatement* TryStatement::Create(Context& context, SourcePosition tryPos, const std::vector<Statement*>& handlers) {
    auto trailSize = GetTrailSize(handlers.size());

    auto* node = new (context.GetAllocator(), trailSize) TryStatement(tryPos, handlers.size());

    node->SetTrailObjects(handlers.data(), handlers.size());

    return node;
}

const ASTNode* TryStatement::GetChild(size_t index) const {
    return (index < m_count) ? At<Statement*>(index) : nullptr;
}

ExceptStatement*
ExceptStatement::Create(Context& context, SourcePosition exceptPos, Identifier _typename, SourcePosition as,
                        Identifier capture, Statement* body) {
    return new (context.GetAllocator()) ExceptStatement(exceptPos, std::move(_typename), as, std::move(capture), body);
}

const ASTNode* ExceptStatement::GetChild(size_t index) const {
    return (index == 0) ? m_body : nullptr;
}

FinallyStatement* FinallyStatement::Create(Context& context, SourcePosition finallyPos, Statement* body) {
    return new (context.GetAllocator()) FinallyStatement(finallyPos, body);
}

const ASTNode* FinallyStatement::GetChild(size_t index) const {
    return (index == 0) ? m_body : nullptr;
}

BreakStatement*
BreakStatement::Create(Context& context, SourcePosition _break, SourcePosition _if, Expression* condition) {
    return new (context.GetAllocator()) BreakStatement(_break, _if, condition);
}

const ASTNode* BreakStatement::GetChild(size_t index) const {
    return (index == 0) ? m_condition : nullptr;
}

ContinueStatement*
ContinueStatement::Create(Context& context, SourcePosition _continue, SourcePosition _if, Expression* condition) {
    return new (context.GetAllocator()) ContinueStatement(_continue, _if, condition);
}

const ASTNode* ContinueStatement::GetChild(size_t index) const {
    return (index == 0) ? m_condition : nullptr;
}

ReturnStatement* ReturnStatement::Create(Context& context, SourcePosition _return, Expression* returnValue) {
    return new (context.GetAllocator()) ReturnStatement(_return, returnValue);
}

const ASTNode* ReturnStatement::GetChild(size_t index) const {
    return (index == 0) ? m_retval : nullptr;
}

AssertStatement*
AssertStatement::Create(Context& context, SourcePosition _assert, Expression* condition, SourcePosition colon,
                        Expression* message) {
    return new (context.GetAllocator()) AssertStatement(_assert, condition, colon, message);
}

const ASTNode* AssertStatement::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_condition;
        case 1: return m_message;
        default: return nullptr;
    }
}

PassStatement* PassStatement::Create(Context& context, SourcePosition pass) {
    return new (context.GetAllocator()) PassStatement(pass);
}

AssignStatement*
AssignStatement::Create(Context& context, Expression* target, AssignOp op, SourcePosition pos, Expression* value) {
    return new (context.GetAllocator()) AssignStatement(target, op, pos, value);
}

const ASTNode* AssignStatement::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_target;
        case 1: return m_value;
        default: return nullptr;
    }
}