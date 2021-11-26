/*
 * Declarations.cpp
 * - Represents declaration in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/Declarations.h>

#include <cassert>

#include <BuildScript/Compiler/AST/Parameters.h>

using namespace BuildScript;

InvalidDeclaration* InvalidDeclaration::Create(Context& context, SourceRange range) {
    return new (context) InvalidDeclaration(range);
}

ScriptDeclaration* ScriptDeclaration::Create(Context& context, std::string name, const std::vector<ASTNode*>& nodes) {
    auto trailSize = GetTrailSize(nodes.size());
    auto range = nodes.empty() ? SourceRange()
                               : SourceRange::Merge(nodes.front()->GetRange(), nodes.back()->GetRange());
    auto* node = new (context, trailSize) ScriptDeclaration(range, std::move(name), nodes.size());

    node->SetTrailObjects<ASTNode*>(nodes.data(), nodes.size());

    return node;
}

const ASTNode* ScriptDeclaration::GetChild(size_t index) const {
    return (index < m_count) ? At<ASTNode*>(index) : nullptr;
}

ImportDeclaration* ImportDeclaration::Create(Context& context, SourcePosition _import, Expression* path) {
    auto range = SourceRange::Merge(_import, path->GetRange());
    return new (context) ImportDeclaration(range, _import, path);
}

const ASTNode* ImportDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_path : nullptr;
}

ExportDeclaration*
ExportDeclaration::Create(Context& context, SourcePosition _export, Identifier name, SourcePosition assign,
                          Expression* value) {
    auto range = SourceRange::Merge(_export, assign ? value->GetRange() : name.GetRange());
    return new (context) ExportDeclaration(range, _export, std::move(name), assign, value);
}

const ASTNode* ExportDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_value : nullptr;
}

FunctionDeclaration*
FunctionDeclaration::Create(Context& context, SourcePosition def, Identifier name, Parameters* param, Statement* body) {
    auto range = SourceRange::Merge(def, body->GetRange());
    return new (context) FunctionDeclaration(range, def, std::move(name), param, body);
}

const ASTNode* FunctionDeclaration::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_params;
        case 1: return m_body;
        default: return nullptr;
    }
}

ClassDeclaration*
ClassDeclaration::Create(Context& context, SourcePosition _class, Identifier name, SourcePosition extends,
                         Identifier extendName, SourcePosition open, const std::vector<Declaration*>& nodes,
                         SourcePosition close) {
    auto trailSize = GetTrailSize(nodes.size());
    auto* node = new (context, trailSize) ClassDeclaration({ _class, close }, _class, std::move(name), extends,
                                                           std::move(extendName), open, close, nodes.size());

    node->SetTrailObjects<Declaration*>(nodes.data(), nodes.size());

    return node;
}

const ASTNode* ClassDeclaration::GetChild(size_t index) const {
    return (index < m_count) ? At<Declaration*>(index) : nullptr;
}

TaskDeclaration*
TaskDeclaration::Create(Context& context, SourcePosition task, Identifier name, SourcePosition extends,
                        Identifier extendName, SourcePosition dependsOn, const std::vector<Identifier>& depnames,
                        const std::vector<SourcePosition>& commas, SourcePosition open,
                        const std::vector<Declaration*>& nodes, SourcePosition close) {
    assert((depnames.size() < 2 && commas.empty()) || ((depnames.size() - 1) == commas.size())
           && "count of dependency names and commas does not match.");

    auto trailSize = GetTrailSize(nodes.size(), depnames.size(), commas.size());
    auto* node = new (context, trailSize) TaskDeclaration({ task, close }, task, std::move(name), extends,
                                                          std::move(extendName), dependsOn, open, close, nodes.size(),
                                                          depnames.size());

    node->SetTrailObjects<Declaration*>(nodes);
    node->SetTrailObjects<Identifier>(depnames);
    node->SetTrailObjects<SourcePosition>(commas);

    return node;
}

const ASTNode* TaskDeclaration::GetChild(size_t index) const {
    return (index < m_count) ? At<Declaration*>(index) : nullptr;
}

VariableDeclaration*
VariableDeclaration::Create(Context& context, SourcePosition _const, SourcePosition var, Identifier name,
                            SourcePosition assign, Expression* value) {
    assert(!(_const && var) && "cannot be both const and var.");
    assert((_const || var) && "cannot be static.");

    SourcePosition pos;
    SpecifierKind kind;

    if (_const) {
        pos = _const;
        kind = SpecifierKind::Const;
    }
    else /* (var) */ {
        pos = var;
        kind = SpecifierKind::Var;
    }

    auto range = SourceRange::Merge(pos, value->GetRange());
    return new (context) VariableDeclaration(range, pos, kind, std::move(name), assign, value);
}

const ASTNode* VariableDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_value : nullptr;
}

TaskInputsDeclaration*
TaskInputsDeclaration::Create(Context& context, SourcePosition inputs, Expression* inputsValue, SourcePosition with,
                              Expression* withValue) {
    auto range = SourceRange::Merge(inputs, with ? withValue->GetRange() : inputsValue->GetRange());
    return new (context) TaskInputsDeclaration(range, inputs, inputsValue, with, withValue);
}

const ASTNode* TaskInputsDeclaration::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_inputsValue;
        case 1: return m_withValue;
        default: return nullptr;
    }
}

TaskOutputsDeclaration*
TaskOutputsDeclaration::Create(Context& context, SourcePosition outputs, Expression* outputsValue, SourcePosition from,
                               Expression* fromValue) {
    auto range = SourceRange::Merge(outputs, from ? fromValue->GetRange() : outputsValue->GetRange());
    return new (context) TaskOutputsDeclaration(range, outputs, outputsValue, from, fromValue);
}

const ASTNode* TaskOutputsDeclaration::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_outputsValue;
        case 1: return m_fromValue;
        default: return nullptr;
    }
}

TaskActionDeclaration*
TaskActionDeclaration::Create(Context& context, ActionKind kind, SourcePosition pos, Statement* body) {
    auto range = SourceRange::Merge(pos, body->GetRange());
    return new (context) TaskActionDeclaration(range, kind, pos, body);
}

const ASTNode* TaskActionDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_body : nullptr;
}

TaskPropertyDeclaration*
TaskPropertyDeclaration::Create(Context& context, Identifier name, SourcePosition assign, Expression* value) {
    auto range = SourceRange::Merge(name.GetRange(), value->GetRange());
    return new (context) TaskPropertyDeclaration(range, std::move(name), assign, value);
}

const ASTNode* TaskPropertyDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_value : nullptr;
}

ClassInitDeclaration*
ClassInitDeclaration::Create(Context& context, SourcePosition init, Parameters* params, Statement* body) {
    auto range = SourceRange::Merge(init, body->GetRange());
    return new (context) ClassInitDeclaration(range, init, params, body);
}

const ASTNode* ClassInitDeclaration::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_params;
        case 1: return m_body;
        default: return nullptr;
    }
}

ClassDeinitDeclaration* ClassDeinitDeclaration::Create(Context& context, SourcePosition deinit, Statement* body) {
    auto range = SourceRange::Merge(deinit, body->GetRange());
    return new (context) ClassDeinitDeclaration(range, deinit, body);
}

const ASTNode* ClassDeinitDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_body : nullptr;
}

ClassFieldDeclaration*
ClassFieldDeclaration::Create(Context& context, SourcePosition _const, SourcePosition _static, Identifier name,
                              SourcePosition assign, Expression* value) {
    assert(!(_const && _static) && "cannot be both const and static.");
    assert((_const || _static) && "cannot be var.");

    SourcePosition pos;
    SpecifierKind kind;

    if (_const) {
        pos = _const;
        kind = SpecifierKind::Const;
    }
    else /* (_static) */ {
        pos = _static;
        kind = SpecifierKind::Static;
    }

    auto range = SourceRange::Merge(pos, value->GetRange());
    return new (context) ClassFieldDeclaration(range, pos, kind, std::move(name), assign, value);
}

const ASTNode* ClassFieldDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_value : nullptr;
}

ClassMethodDeclaration*
ClassMethodDeclaration::Create(Context& context, SourcePosition _static, SourcePosition def, Identifier name,
                               Parameters* params, Statement* body) {
    auto range = SourceRange::Merge((_static < def ? _static : def), body->GetRange());
    return new (context) ClassMethodDeclaration(range, _static, def, std::move(name), params, body);
}

const ASTNode* ClassMethodDeclaration::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_params;
        case 1: return m_body;
        default: return nullptr;
    }
}

ClassPropertyDeclaration*
ClassPropertyDeclaration::Create(Context& context, SourcePosition get, SourcePosition set, Identifier name,
                                 Statement* body) {
    assert(!(get && set) && "cannot be both getter and setter.");

    auto isGetter = (bool) get;
    auto pos = isGetter ? get : set;
    auto range = SourceRange::Merge(pos, body->GetRange());
    return new (context) ClassPropertyDeclaration(range, pos, std::move(name), isGetter, body);
}

const ASTNode* ClassPropertyDeclaration::GetChild(size_t index) const {
    return (index == 0) ? m_body : nullptr;
}

ClassOperatorDeclaration*
ClassOperatorDeclaration::Create(Context& context, SourcePosition _operator, OperatorKind kind,
                                 std::array<SourcePosition, 2> pos, Parameters* params, Statement* body) {
    auto range = SourceRange::Merge(_operator, body->GetRange());
    return new (context) ClassOperatorDeclaration(range, _operator, kind, pos, params, body);
}

const ASTNode* ClassOperatorDeclaration::GetChild(size_t index) const {
    switch (index) {
        case 0: return m_params;
        case 1: return m_body;
        default: return nullptr;
    }
}