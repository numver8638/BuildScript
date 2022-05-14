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

ArrowStatement* ArrowStatement::Create(Context& context, SourcePosition arrow, Expression* expr) {
    return new (context.GetAllocator()) ArrowStatement(arrow, expr);
}

IfStatement*
IfStatement::Create(Context& context, SourcePosition _if, Expression* condition, Statement* ifBody,
                    SourcePosition _else, Statement* elseBody) {
    return new (context.GetAllocator()) IfStatement(_if, condition, ifBody, _else, elseBody);
}

MatchStatement*
MatchStatement::Create(Context& context, SourcePosition match, Expression* condition, SourcePosition open,
                       const std::vector<Statement*>& nodes, SourcePosition close) {
    auto trailSize = GetTrailSize(nodes.size());
    auto* node = new (context.GetAllocator(), trailSize) MatchStatement(match, condition, open, close, nodes.size());

    node->SetTrailObjects(nodes.data(), nodes.size());

    return node;
}

Label*
Label::Create(Context& context, SourcePosition _case, SourcePosition _default, Expression* expr, SourcePosition colon) {
    assert(!(_case && _default) && "cannot be both case label and default label.");

    auto pos = _default ? _default : _case;
    return new (context.GetAllocator()) Label(pos, expr, colon, (bool)_default);
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

ForStatement*
ForStatement::Create(Context& context, SourcePosition _for, Parameter* param, SourcePosition _in, Expression* expr,
                     Statement* body) {
    auto* node = new (context.GetAllocator()) ForStatement(_for, param, _in, expr, body);

    return node;
}

WhileStatement*
WhileStatement::Create(Context& context, SourcePosition _while, Expression* condition, Statement* body) {
    return new (context.GetAllocator()) WhileStatement(_while, condition, body);
}

WithStatement*
WithStatement::Create(Context& context, SourcePosition with, Expression* expr, SourcePosition as, Parameter* capture,
                      Statement* body) {
    return new (context.GetAllocator()) WithStatement(with, expr, as, capture, body);
}

TryStatement* TryStatement::Create(Context& context, SourcePosition tryPos, const std::vector<Statement*>& handlers) {
    auto trailSize = GetTrailSize(handlers.size());

    auto* node = new (context.GetAllocator(), trailSize) TryStatement(tryPos, handlers.size());

    node->SetTrailObjects(handlers.data(), handlers.size());

    return node;
}

ExceptStatement*
ExceptStatement::Create(Context& context, SourcePosition exceptPos, Identifier _typename, SourcePosition as,
                        Parameter* capture, Statement* body) {
    return new (context.GetAllocator()) ExceptStatement(exceptPos, std::move(_typename), as, capture, body);
}

FinallyStatement* FinallyStatement::Create(Context& context, SourcePosition finallyPos, Statement* body) {
    return new (context.GetAllocator()) FinallyStatement(finallyPos, body);
}

BreakStatement*
BreakStatement::Create(Context& context, SourcePosition _break, SourcePosition _if, Expression* condition) {
    return new (context.GetAllocator()) BreakStatement(_break, _if, condition);
}

ContinueStatement*
ContinueStatement::Create(Context& context, SourcePosition _continue, SourcePosition _if, Expression* condition) {
    return new (context.GetAllocator()) ContinueStatement(_continue, _if, condition);
}

ReturnStatement* ReturnStatement::Create(Context& context, SourcePosition _return, Expression* returnValue) {
    return new (context.GetAllocator()) ReturnStatement(_return, returnValue);
}

AssertStatement*
AssertStatement::Create(Context& context, SourcePosition _assert, Expression* condition, SourcePosition colon,
                        Expression* message) {
    return new (context.GetAllocator()) AssertStatement(_assert, condition, colon, message);
}

PassStatement* PassStatement::Create(Context& context, SourcePosition pass) {
    return new (context.GetAllocator()) PassStatement(pass);
}

AssignStatement*
AssignStatement::Create(Context& context, Expression* target, AssignOp op, SourcePosition pos, Expression* value) {
    return new (context.GetAllocator()) AssignStatement(target, op, pos, value);
}