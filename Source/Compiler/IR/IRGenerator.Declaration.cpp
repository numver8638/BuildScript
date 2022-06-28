/*
 * IRGenerator.Declaration.cpp
 * - Intermediate Representation code generator.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRGenerator.h>

#include <algorithm>
#include <iterator>
#include <fmt/format.h>

#include <BuildScript/Compiler/Symbol/Symbol.h>
#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/ParameterList.h>
#include <BuildScript/Compiler/IR/Statements.h>

using namespace BuildScript;

constexpr std::string_view ClassInitNameTemplate = "{0}.<cinit>$0";
constexpr std::string_view InitNameTemplate = "{0}.<init>$1";
constexpr std::string_view DeinitNameTemplate = "{0}.<deinit>$1";

inline bool IsGlobal(Symbol* symbol) {
    if (auto* var = symbol->As<VariableSymbol>()) {
        return (var->GetVariableType() == BuildScript::VariableType::Global)
               || (var->GetVariableType() == BuildScript::VariableType::Exported);
    }

    return false;
}

void IRGenerator::Walk(Parameter* node) {
    // do nothing
}

void IRGenerator::Walk(InvalidDeclaration* node) {
    NEVER_BE_CALLED("cannot be generated with erroneous script.");
}

void IRGenerator::Walk(ScriptDeclaration* node) {
    PushBuilder("<script>", {}, false);

    super::Walk(node);

    INSERT_EMPTY_RETURN;

    PopBuilder();
}

void IRGenerator::Walk(ImportDeclaration* node) {
    auto path = EvaluateExpr(node->GetPath());

    GetBuilder().InsertBack<IRImportOp>(node->GetImportPosition(), path);
}

void IRGenerator::Walk(ExportDeclaration* node) {
    Symbol* symbol = node->GetSymbol();

    if (node->HasValue()) {
        auto value = EvaluateExpr(node->GetValue());

        assert(IsGlobal(node->GetSymbol()));

        GetBuilder().InsertBack<IRDeclareSymbolOp>(node->GetAssignPosition(), node->GetSymbol(), value);

        // Add symbol into defined table manually; WriteSymbol() method may add unnecessary StoreSymbol op.
        GetBuilder().GetCurrentBlock().Defined.insert_or_assign(node->GetSymbol(), value);
    }

    GetBuilder().InsertBack<IRExportOp>(node->GetExportPosition(), symbol);
}

void IRGenerator::Walk(FunctionDeclaration* node) {
    std::vector<Symbol*> args;
    bool vararg = node->GetParameterList()->HasVariadicArgument();
    auto params = node->GetParameterList()->GetParameters();

    std::transform(params.begin(), params.end(), std::back_inserter(args), [](auto* param) {
        return param->GetSymbol();
    });

    PushBuilder(node->GetSymbol()->GetMangledName(), args, vararg);

    GenerateBody(node->GetBody());

    PopBuilder();

    GetBuilder().InsertBack<IRDeclareSymbolOp>(node->GetDefPosition(), node->GetSymbol(), IRInvalidValue);
}

void IRGenerator::Walk(ClassDeclaration* node) {
    auto* symbol = node->GetSymbol();

    {
        PushBuilder(fmt::format(ClassInitNameTemplate, symbol->GetName()), {}, false);

        super::Walk(node);

        INSERT_EMPTY_RETURN;

        PopBuilder();
    }

    // Check the class has initializer. If the class does not have initializer, insert empty initializer.
    auto members = node->GetMembers();
    auto hasInit = std::any_of(members.begin(), members.end(),
                               [](auto* node) { return node->GetKind() == DeclarationKind::ClassInit; });
    auto hasDeinit = std::any_of(members.begin(), members.end(),
                                 [](auto* node) { return node->GetKind() == DeclarationKind::ClassDeinit; });
    if (!hasInit) {
        PushBuilder(fmt::format(InitNameTemplate, symbol->GetName()), { VariableSymbol::GetSelf() }, false);

        auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSuper());
        auto name = Value{ std::string{ Symbol::InitializerName } };
        GenerateInvoke(SourcePosition(), target, std::move(name), {});

        INSERT_EMPTY_RETURN;
        PopBuilder();
    }

    if (!hasDeinit) {
        PushBuilder(fmt::format(DeinitNameTemplate, symbol->GetName()), { VariableSymbol::GetSelf() }, false);

        auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSuper());
        auto name = Value{ std::string{ Symbol::DeinitializerName } };
        GenerateInvoke(SourcePosition(), target, std::move(name), {});

        INSERT_EMPTY_RETURN;
        PopBuilder();
    }

    GetBuilder().InsertBack<IRDeclareSymbolOp>(node->GetClassPosition(), node->GetSymbol(), IRInvalidValue);
}

void IRGenerator::Walk(TaskDeclaration* node) {
    // Desugar declaration: translate task declaration to class declaration.
    std::vector<Declaration*> pendingDecls;
    {
        PushBuilder(fmt::format(ClassInitNameTemplate, node->GetSymbol()->GetName()), {}, false);
        for (auto* decl: node->GetMembers()) {
            if (decl->GetKind() == DeclarationKind::TaskInputs || decl->GetKind() == DeclarationKind::TaskOutputs) {
                pendingDecls.push_back(decl); // pending IR generation to init.
            }
            else {
                super::Walk(decl);
            }
        }

        INSERT_EMPTY_RETURN;
        PopBuilder();
    }
    {
        PushBuilder(fmt::format(InitNameTemplate, node->GetSymbol()->GetName()), { VariableSymbol::GetSelf() }, false);

        auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSuper());
        auto name = Value{ std::string{ Symbol::InitializerName } };
        auto arg = GetBuilder().ReadConst(SourcePosition(), Value{ std::string{ node->GetName().GetString() }});
        GenerateInvoke(SourcePosition(), target, std::move(name), { arg });

        for (auto* decl: pendingDecls) {
            super::Walk(decl);
        }

        INSERT_EMPTY_RETURN;
        PopBuilder();
    }

    GetBuilder().InsertBack<IRDeclareSymbolOp>(node->GetTaskPosition(), node->GetSymbol(), IRInvalidValue);
}

void IRGenerator::Walk(VariableDeclaration* node) {
    auto value = EvaluateExpr(node->GetValue());

    if (IsGlobal(node->GetSymbol())) {
        GetBuilder().InsertBack<IRDeclareSymbolOp>(node->GetAssignPosition(), node->GetSymbol(), value);
    }

    // Add symbol into defined table manually; WriteSymbol() method may add unnecessary StoreSymbol op.
    GetBuilder().GetCurrentBlock().Defined.insert_or_assign(node->GetSymbol(), value);
}

void IRGenerator::Walk(TaskInputsDeclaration* node) {
    // Desugar declaration: convert `inputs ... with ...` to invocations in initializer.
    static auto Inputs = Value{ std::string{ "Inputs" } };
    static auto Resolver = Value{ std::string{ "Resolver" } };

    auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSelf());
    auto inputs = EvaluateExpr(node->GetInputsValue());
    GenerateInvoke(node->GetInputsPosition(), target, Inputs, { inputs });

    if (node->HasWith()) {
        auto with = EvaluateExpr(node->GetWithValue());
        GenerateInvoke(node->GetWithPosition(), target, Resolver, { with });
    }
}

void IRGenerator::Walk(TaskOutputsDeclaration* node) {
    // Desugar declaration: convert `outputs ... from ...` to invocations in initializer.
    static auto Outputs = Value{ std::string{ "Outputs" } };
    static auto Pattern = Value{ std::string{ "Pattern" } };

    auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSelf());
    if (node->HasFrom()) {
        // `outputs ... from ...` converts to `Pattern(..., ...)`
        auto outputs = EvaluateExpr(node->GetOutputsValue());
        auto from = EvaluateExpr(node->GetFromValue());
        GenerateInvoke(node->GetOutputsPosition(), target, Pattern, { outputs, from });
    }
    else {
        // `outputs ...` converts to `Outputs(...)`
        auto outputs = EvaluateExpr(node->GetOutputsValue());
        GenerateInvoke(node->GetOutputsPosition(), target, Outputs, { outputs });
    }
}

void IRGenerator::Walk(TaskActionDeclaration* node) {
    // Desugar declaration: convert action to method declaration.
    std::vector<Symbol*> args{ VariableSymbol::GetSelf() };
    bool vararg = node->GetParameterList()->HasVariadicArgument();
    auto params = node->GetParameterList()->GetParameters();

    std::transform(params.begin(), params.end(), std::back_inserter(args), [](auto* param) {
        return param->GetSymbol();
    });

    assert((node->GetSymbol()->As<MethodSymbol>()->GetArgumentCount() == args.size()));

    PushBuilder(node->GetSymbol()->GetMangledName(), args, vararg);

    GenerateBody(node->GetBody());

    PopBuilder();
}

void IRGenerator::Walk(TaskPropertyDeclaration* node) {
    // Desugar declaration: convert declaration to field declaration.
    auto value = EvaluateExpr(node->GetValue());

    GetBuilder().InsertBack<IRDeclareSymbolOp>(node->GetAssignPosition(), node->GetSymbol(), value);
}

void IRGenerator::Walk(ClassInitDeclaration* node) {
    std::vector<Symbol*> args{ VariableSymbol::GetSelf() };
    bool vararg = node->GetParameterList()->HasVariadicArgument();
    auto params = node->GetParameterList()->GetParameters();

    std::transform(params.begin(), params.end(), std::back_inserter(args), [](auto* param) {
        return param->GetSymbol();
    });

    assert((node->GetSymbol()->As<MethodSymbol>()->GetArgumentCount() == args.size()));

    PushBuilder(node->GetSymbol()->GetMangledName(), args, vararg);

    if (!node->HasInitializerCall()) {
        auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSuper());
        auto name = Value{ std::string{ MethodSymbol::InitializerName } };
        GenerateInvoke(SourcePosition(), target, std::move(name), {});
    }

    GenerateBody(node->GetBody());

    PopBuilder();
}

void IRGenerator::Walk(ClassDeinitDeclaration* node) {
    assert((node->GetSymbol()->As<MethodSymbol>()->GetArgumentCount() == 1));

    PushBuilder(node->GetSymbol()->GetMangledName(), { VariableSymbol::GetSelf() }, false);

    GenerateBody(node->GetBody());

    auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSelf());
    auto name = Value{ std::string{ MethodSymbol::DeinitializerName } };
    GenerateInvoke(SourcePosition(), target, std::move(name), {});

    PopBuilder();
}

void IRGenerator::Walk(ClassFieldDeclaration* node) {
    auto value = EvaluateExpr(node->GetValue());

    GetBuilder().InsertBack<IRDeclareSymbolOp>(node->GetAssignPosition(), node->GetSymbol(), value);
}

void IRGenerator::Walk(ClassMethodDeclaration* node) {
    auto IsInstanceMethod = [](ClassMethodDeclaration* node) {
        if (node->IsOperator()) {
            return (OperatorKind::InplaceAdd <= node->GetOperator())
                && (node->GetOperator() <= OperatorKind::InplaceBitXor);
        }
        else {
            return !node->IsStatic();
        }
    };
    std::vector<Symbol*> args;

    if (IsInstanceMethod(node)) {
        args.push_back(VariableSymbol::GetSelf());
    }

    bool vararg = node->GetParameterList()->HasVariadicArgument();
    auto params = node->GetParameterList()->GetParameters();

    std::transform(params.begin(), params.end(), std::back_inserter(args), [](auto* param) {
        return param->GetSymbol();
    });

    assert((node->GetSymbol()->As<MethodSymbol>()->GetArgumentCount() == args.size()));

    PushBuilder(node->GetSymbol()->GetMangledName(), args, vararg);

    GenerateBody(node->GetBody());

    PopBuilder();
}

void IRGenerator::Walk(ClassPropertyDeclaration* node) {
    std::vector<Symbol*> args{ VariableSymbol::GetSelf() };

    bool vararg = node->GetParameterList()->HasVariadicArgument();
    auto params = node->GetParameterList()->GetParameters();

    std::transform(params.begin(), params.end(), std::back_inserter(args), [](auto* param) {
        return param->GetSymbol();
    });

    PushBuilder(node->GetSymbol()->GetMangledName(), args, vararg);

    GenerateBody(node->GetBody());

    PopBuilder();
}