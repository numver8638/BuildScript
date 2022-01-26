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
#include <BuildScript/Compiler/Context.h>

using namespace BuildScript;

InvalidDeclaration* InvalidDeclaration::Create(Context& context, SourceRange range) {
    return new (context.GetAllocator()) InvalidDeclaration(range);
}

ScriptDeclaration* ScriptDeclaration::Create(Context& context, std::string name, const std::vector<ASTNode*>& nodes) {
    auto trailSize = GetTrailSize(nodes.size());
    auto* node = new (context.GetAllocator(), trailSize) ScriptDeclaration(std::move(name), nodes.size());

    node->SetTrailObjects<ASTNode*>(nodes.data(), nodes.size());

    return node;
}

ImportDeclaration* ImportDeclaration::Create(Context& context, SourcePosition _import, Expression* path) {
    return new (context.GetAllocator()) ImportDeclaration(_import, path);
}

ExportDeclaration*
ExportDeclaration::Create(Context& context, SourcePosition _export, Identifier name, SourcePosition assign,
                          Expression* value) {
    return new (context.GetAllocator()) ExportDeclaration(_export, std::move(name), assign, value);
}

FunctionDeclaration*
FunctionDeclaration::Create(Context& context, SourcePosition def, Identifier name, Parameters* param, Statement* body) {
    return new (context.GetAllocator()) FunctionDeclaration(def, std::move(name), param, body);
}

ClassDeclaration*
ClassDeclaration::Create(Context& context, SourcePosition _class, Identifier name, SourcePosition extends,
                         Identifier extendName, SourcePosition open, const std::vector<Declaration*>& nodes,
                         SourcePosition close) {
    auto trailSize = GetTrailSize(nodes.size());
    auto* node = new (context.GetAllocator(), trailSize) ClassDeclaration(_class, std::move(name), extends,
                                                           std::move(extendName), open, close, nodes.size());

    node->SetTrailObjects<Declaration*>(nodes.data(), nodes.size());

    return node;
}

TaskDeclaration*
TaskDeclaration::Create(Context& context, SourcePosition task, Identifier name, SourcePosition extends,
                        Identifier extendName, SourcePosition dependsOn, const std::vector<Identifier>& depnames,
                        const std::vector<SourcePosition>& commas, SourcePosition open,
                        const std::vector<Declaration*>& nodes, SourcePosition close) {
    assert((depnames.size() < 2 && commas.empty()) || ((depnames.size() - 1) == commas.size())
           && "count of dependency names and commas does not match.");

    auto trailSize = GetTrailSize(nodes.size(), depnames.size(), commas.size());
    auto* node = new (context.GetAllocator(), trailSize) TaskDeclaration(task, std::move(name), extends,
                                                          std::move(extendName), dependsOn, open, close, nodes.size(),
                                                          depnames.size());

    node->SetTrailObjects<Declaration*>(nodes);
    node->SetTrailObjects<Identifier>(depnames);
    node->SetTrailObjects<SourcePosition>(commas);

    return node;
}

VariableDeclaration*
VariableDeclaration::Create(Context& context, SourcePosition _const, SourcePosition var, Identifier name,
                            SourcePosition assign, Expression* value) {
    assert(!(_const && var) && "cannot be both const and var.");

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

    return new (context.GetAllocator()) VariableDeclaration(pos, kind, std::move(name), assign, value);
}

TaskInputsDeclaration*
TaskInputsDeclaration::Create(Context& context, SourcePosition inputs, Expression* inputsValue, SourcePosition with,
                              Expression* withValue) {
    return new (context.GetAllocator()) TaskInputsDeclaration(inputs, inputsValue, with, withValue);
}

TaskOutputsDeclaration*
TaskOutputsDeclaration::Create(Context& context, SourcePosition outputs, Expression* outputsValue, SourcePosition from,
                               Expression* fromValue) {
    return new (context.GetAllocator()) TaskOutputsDeclaration(outputs, outputsValue, from, fromValue);
}

TaskActionDeclaration*
TaskActionDeclaration::Create(Context& context, ActionKind kind, SourcePosition pos, Statement* body) {
    return new (context.GetAllocator()) TaskActionDeclaration(kind, pos, body);
}

TaskPropertyDeclaration*
TaskPropertyDeclaration::Create(Context& context, Identifier name, SourcePosition assign, Expression* value) {
    return new (context.GetAllocator()) TaskPropertyDeclaration(std::move(name), assign, value);
}

ClassInitDeclaration*
ClassInitDeclaration::Create(Context& context, SourcePosition init, Parameters* params, Statement* body) {
    return new (context.GetAllocator()) ClassInitDeclaration(init, params, body);
}

ClassDeinitDeclaration* ClassDeinitDeclaration::Create(Context& context, SourcePosition deinit, Statement* body) {
    return new (context.GetAllocator()) ClassDeinitDeclaration(deinit, body);
}

ClassFieldDeclaration*
ClassFieldDeclaration::Create(Context& context, SourcePosition _const, SourcePosition _static, Identifier name,
                              SourcePosition assign, Expression* value) {
    assert(!(_const && _static) && "cannot be both const and static.");

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

    return new (context.GetAllocator()) ClassFieldDeclaration(pos, kind, std::move(name), assign, value);
}

ClassMethodDeclaration*
ClassMethodDeclaration::Create(Context& context, SourcePosition _static, SourcePosition def, Identifier name,
                               Parameters* params, Statement* body) {
    return new (context.GetAllocator()) ClassMethodDeclaration(_static, def, std::move(name), params, body);
}

ClassPropertyDeclaration*
ClassPropertyDeclaration::Create(Context& context, SourcePosition get, SourcePosition set, Identifier name,
                                 Statement* body) {
    assert(!(get && set) && "cannot be both getter and setter.");

    auto isGetter = (bool) get;
    auto pos = isGetter ? get : set;
    return new (context.GetAllocator()) ClassPropertyDeclaration(pos, std::move(name), isGetter, body);
}

ClassOperatorDeclaration*
ClassOperatorDeclaration::Create(Context& context, SourcePosition _operator, OperatorKind kind,
                                 std::array<SourcePosition, 2> pos, Parameters* params, Statement* body) {
    return new (context.GetAllocator()) ClassOperatorDeclaration(_operator, kind, pos, params, body);
}