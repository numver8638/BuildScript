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

#include <fmt/format.h>

#include <BuildScript/Compiler/Context.h>
#include <BuildScript/Compiler/AST/ParameterList.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>

using namespace BuildScript;

InvalidDeclaration* InvalidDeclaration::Create(Context& context, SourceRange range) {
    return new (context.GetAllocator()) InvalidDeclaration(range);
}

Parameter* Parameter::Create(Context& context, Identifier name) {
    return new (context.GetAllocator()) Parameter(std::move(name));
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
FunctionDeclaration::Create(Context& context, SourcePosition def, Identifier name, ParameterList* param, Statement* body) {
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
VariableDeclaration::Create(Context& context, SourcePosition pos, AccessFlags flags, Identifier name,
                            SourcePosition assign, Expression* value) {
    assert((flags != AccessFlags::Static) && "cannot be static.");

    return new (context.GetAllocator()) VariableDeclaration(pos, flags, std::move(name), assign, value);
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
    auto argc = (kind == ActionKind::Do) ? 4 : 2;
    std::vector<Parameter*> args;

    static const std::string_view symbols[] = {
        VariableSymbol::Inputs,
        VariableSymbol::Outputs,
        VariableSymbol::Input,
        VariableSymbol::Output,
    };

    for (auto i = 0; i < argc; i++) {
        args.push_back(Parameter::Create(context, Identifier(SourceRange(), std::string(symbols[i]))));
    }

    auto* params = ParameterList::Create(context, SourcePosition(), args, std::vector(argc - 1, SourcePosition()),
                                         SourcePosition(), SourcePosition());

    return new (context.GetAllocator()) TaskActionDeclaration(kind, pos, body, params);
}

TaskPropertyDeclaration*
TaskPropertyDeclaration::Create(Context& context, Identifier name, SourcePosition assign, Expression* value) {
    return new (context.GetAllocator()) TaskPropertyDeclaration(std::move(name), assign, value);
}

ClassInitDeclaration*
ClassInitDeclaration::Create(Context& context, SourcePosition init, ParameterList* params, Statement* body) {
    return new (context.GetAllocator()) ClassInitDeclaration(init, params, body);
}

ClassDeinitDeclaration* ClassDeinitDeclaration::Create(Context& context, SourcePosition deinit, Statement* body) {
    return new (context.GetAllocator()) ClassDeinitDeclaration(deinit, body);
}

ClassFieldDeclaration*
ClassFieldDeclaration::Create(Context& context, SourcePosition pos, AccessFlags flags, Identifier name,
                              SourcePosition assign, Expression* value) {
    assert((flags != AccessFlags::ReadWrite) && "cannot be var.");

    return new (context.GetAllocator()) ClassFieldDeclaration(pos, flags, std::move(name), assign, value);
}

ClassMethodDeclaration*
ClassMethodDeclaration::CreateMethod(Context& context, SourcePosition _static, SourcePosition def, Identifier name,
                               ParameterList* params, Statement* body) {
    return new (context.GetAllocator()) ClassMethodDeclaration(_static, def, std::move(name), OperatorKind::Invalid,
                                                               SourcePosition::Empty(), params, body);
}

ClassMethodDeclaration*
ClassMethodDeclaration::CreateOperator(Context& context, SourcePosition def, OperatorKind op, SourcePosition pos,
                                       ParameterList* params, Statement* body) {
    auto name = Identifier{ SourceRange(), fmt::format("<operator>{0}", OperatorKindToString(op)) };
    return new (context.GetAllocator()) ClassMethodDeclaration(SourcePosition::Empty(), def, std::move(name),
                                                               op, pos, params, body);
}

ClassPropertyDeclaration*
ClassPropertyDeclaration::Create(Context& context, SourcePosition get, SourcePosition set, Identifier name,
                                 Statement* body) {
    assert(!(get && set) && "cannot be both getter and setter.");

    auto isGetter = (bool) get;
    auto pos = isGetter ? get : set;

    std::vector<Parameter*> args;
    args.push_back(Parameter::Create(context, Identifier(SourceRange(), std::string(VariableSymbol::Value))));

    auto* params = ParameterList::Create(context, SourcePosition(), args, {}, SourcePosition(), SourcePosition());

    return new (context.GetAllocator()) ClassPropertyDeclaration(pos, std::move(name), SourcePosition::Empty(),
                                                                 isGetter, body, params);
}

ClassPropertyDeclaration*
ClassPropertyDeclaration::CreateSubscript(Context& context, SourcePosition get, SourcePosition set,
                                          SourcePosition subscript, Statement* body) {
    static const auto SUBSCRIPT_NAME = Identifier{ SourceRange(), std::string(Symbol::SubscriptName) };
    auto isGetter = (bool) get;
    auto pos = isGetter ? get : set;

    std::vector<Parameter*> args;
    args.push_back(Parameter::Create(context, Identifier(SourceRange(), std::string(VariableSymbol::Index))));
    args.push_back(Parameter::Create(context, Identifier(SourceRange(), std::string(VariableSymbol::Value))));

    auto* params = ParameterList::Create(context, SourcePosition(), args, { SourcePosition() }, SourcePosition(),
                                         SourcePosition());

    return new (context.GetAllocator()) ClassPropertyDeclaration(pos, SUBSCRIPT_NAME, subscript, isGetter, body, params);
}