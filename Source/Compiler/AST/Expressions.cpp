/*
 * Expressions.cpp
 * - Represents expression in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/Expressions.h>

#include <BuildScript/Compiler/AST/Parameters.h>

using namespace BuildScript;

InvalidExpression* InvalidExpression::Create(Context& context, SourceRange range) {
    return new (context) InvalidExpression(range);
}

PassExpression* PassExpression::Create(Context& context, SourceRange range) {
    return new (context) PassExpression(range);
}

TernaryExpression*
TernaryExpression::Create(Context& context, Expression* valueT, SourcePosition _if, Expression* condition,
                          SourcePosition _else, Expression* valueF) {
    auto range = SourceRange::Merge(valueT->GetRange(), valueF->GetRange());
    return new (context) TernaryExpression(range, valueT, _if, condition, _else, valueF);
}

const ASTNode* TernaryExpression::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_valueT;
        case 1: return m_cond;
        case 2: return m_valueF;
        default: return nullptr;
    }
}

BinaryExpression*
BinaryExpression::Create(Context& context, Expression* left, BinaryOp op, std::array<SourcePosition, 2> pos,
                         Expression* right) {
    auto range = SourceRange::Merge(left->GetRange(), right->GetRange());
    return new (context) BinaryExpression(range, left, op, pos, right);
}

const ASTNode* BinaryExpression::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_left;
        case 1: return m_right;
        default: return nullptr;
    }
}

UnaryExpression* UnaryExpression::Create(Context& context, UnaryOp op, SourcePosition pos, Expression* expr) {
    auto range = SourceRange::Merge(pos, expr->GetRange());
    return new (context) UnaryExpression(range, op, pos, expr);
}

const ASTNode* UnaryExpression::GetChild(size_t index) const {
    return (index == 0) ? m_expr : nullptr;
}

DefinedExpression*
DefinedExpression::Create(Context& context, SourcePosition defined, Identifier id, SourcePosition in,
                          Expression* expr) {
    auto range = SourceRange::Merge(defined, in ? expr->GetRange() : id.GetRange());
    return new (context) DefinedExpression(range, defined, std::move(id), in, expr);
}

const ASTNode* DefinedExpression::GetChild(size_t index) const {
    return (index == 0) ? m_target : nullptr;
}

RaiseExpression* RaiseExpression::Create(Context& context, SourcePosition raise, Expression* expr) {
    auto range = SourceRange::Merge(raise, expr->GetRange());
    return new (context) RaiseExpression(range, raise, expr);
}

const ASTNode* RaiseExpression::GetChild(size_t index) const {
    return (index == 0) ? m_target : nullptr;
}

MemberAccessExpression*
MemberAccessExpression::Create(Context& context, Expression* target, SourcePosition dot, Identifier name) {
    auto range = SourceRange::Merge(target->GetRange(), name.GetRange());
    return new (context) MemberAccessExpression(range, target, dot, std::move(name));
}

const ASTNode* MemberAccessExpression::GetChild(size_t index) const {
    return (index == 0) ? m_target : nullptr;
}

InvocationExpression*
InvocationExpression::Create(Context& context, SourcePosition open, const std::vector<Expression*>& nodes,
                             const std::vector<SourcePosition>& commas, SourcePosition close) {
    assert(!nodes.empty() && "never be empty.");
    assert(((nodes.size() < 3 && commas.empty()) || ((nodes.size() - 2) == commas.size()))
            && "count of expressions and commas does not match.");

    auto trailSize = GetTrailSize(nodes.size(), commas.size());
    auto range = SourceRange::Merge(nodes.front()->GetRange(), close);
    auto* node = new (context, trailSize) InvocationExpression(range, open, close, nodes.size(), commas.size());

    node->SetTrailObjects(nodes.data(), nodes.size());
    node->SetTrailObjects(commas.data(), commas.size());

    return node;
}

const ASTNode* InvocationExpression::GetChild(size_t index) const {
    return (index < m_exprCount) ? At<Expression*>(index) : nullptr;
}

SubscriptExpression*
SubscriptExpression::Create(Context& context, Expression* target, SourcePosition open, Expression* index,
                            SourcePosition close) {
    auto range = SourceRange::Merge(target->GetRange(), close);
    return new (context) SubscriptExpression(range, target, open, index, close);
}

const ASTNode* SubscriptExpression::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_target;
        case 1: return m_index;
        default: return nullptr;
    }
}

ParenthesizedExpression*
ParenthesizedExpression::Create(Context& context, SourcePosition open, Expression* expr, SourcePosition close) {
    return new (context) ParenthesizedExpression({open, close}, open, close, expr);
}

const ASTNode* ParenthesizedExpression::GetChild(size_t index) const {
    return (index == 0) ? m_expr : nullptr;
}

ListExpression*
ListExpression::Create(Context& context, SourcePosition open, const std::vector<Expression*>& items,
                       const std::vector<SourcePosition>& commas, SourcePosition close) {
    assert(((items.size() < 2 && commas.empty()) || ((items.size() - 1) == commas.size()))
            && "count of expressions and commas does not match.");

    auto trailSize = GetTrailSize(items.size(), commas.size());
    auto* node = new (context, trailSize) ListExpression({ open, close }, open, close, items.size());

    node->SetTrailObjects(items.data(), items.size());
    node->SetTrailObjects(commas.data(), commas.size());

    return node;
}

const ASTNode* ListExpression::GetChild(size_t index) const {
    return (index < m_count) ? At<Expression*>(index) : nullptr;
}

KeyValuePair* KeyValuePair::Create(Context& context, Expression* key, SourcePosition colon, Expression* value) {
    auto range = SourceRange::Merge(key->GetRange(), value->GetRange());
    return new (context) KeyValuePair(range, key, colon, value);
}

const ASTNode* KeyValuePair::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_key;
        case 1: return m_value;
        default: return nullptr;
    }
}

MapExpression*
MapExpression::Create(Context& context, SourcePosition open, const std::vector<Expression*>& items,
                      const std::vector<SourcePosition>& commas, SourcePosition close) {
    assert(((items.size() < 2 && commas.empty()) || ((items.size() - 1) == commas.size()))
            && "count of expressions and commas does not match.");

    auto trailSize = GetTrailSize(items.size(), commas.size());
    auto* node = new (context, trailSize) MapExpression({ open, close }, open, close, items.size());

    node->SetTrailObjects(items.data(), items.size());
    node->SetTrailObjects(commas.data(), commas.size());

    return node;
}

ASTNode* MapExpression::GetChild(size_t index) const {
    return (index < m_count) ? At<Expression*>(index) : nullptr;
}

ClosureExpression*
ClosureExpression::Create(Context& context, Parameters* params, SourcePosition arrow, ASTNode* body) {
    auto range = SourceRange::Merge(params->GetRange(), body->GetRange());
    return new (context) ClosureExpression(range, arrow, params, body);
}

ASTNode* ClosureExpression::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_params;
        case 1: return m_body;
        default: return nullptr;
    }
}

LiteralExpression* LiteralExpression::CreateVariable(Context& context, const Identifier& id) {
    return new (context) LiteralExpression(id.GetRange(), LiteralType::Variable, { id.GetString() });
}

LiteralExpression* LiteralExpression::CreateInteger(Context& context, SourceRange range, int64_t value) {
    return new (context) LiteralExpression(range, LiteralType::Integer, { value });
}

LiteralExpression* LiteralExpression::CreateFloat(Context& context, SourceRange range, double value) {
    return new (context) LiteralExpression(range, LiteralType::Float, { value });
}

LiteralExpression* LiteralExpression::CreateBoolean(Context& context, SourceRange range, bool value) {
    return new (context) LiteralExpression(range, LiteralType::Boolean, { value });
}

LiteralExpression*
LiteralExpression::CreateString(Context& context, SourceRange range, std::string value,
                                const std::vector<Expression*>& exprs) {
    auto trailSize = GetTrailSize(exprs.size());

    auto* node = new (context, trailSize) LiteralExpression(range, LiteralType::String,
                                                            { std::move(value) }, exprs.size());
    node->SetTrailObjects(exprs.data(), exprs.size());

    return node;
}

LiteralExpression* LiteralExpression::CreateNone(Context& context, SourceRange range) {
    return new (context) LiteralExpression(range, LiteralType::None, {});
}

LiteralExpression* LiteralExpression::CreateSelf(Context& context, SourceRange range) {
    return new (context) LiteralExpression(range, LiteralType::Self, {});
}

LiteralExpression* LiteralExpression::CreateSuper(Context& context, SourceRange range) {
    return new (context) LiteralExpression(range, LiteralType::Super, {});
}

ASTNode* LiteralExpression::GetChild(size_t index) const {
    return (m_type == LiteralType::String && (index < m_count)) ? At<Expression*>(index) : nullptr;
}