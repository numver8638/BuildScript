/*
 * SemanticAnalyzer.Declaration.cpp
 * - Verify and analyze AST given by Parser.
 *   This file contains part of SemanticAnalyzer that check declarations.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.h>

#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/ParameterList.h>
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.AutoScope.h>
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.Builder.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>
#include <BuildScript/Utils/Range.h>

using namespace BuildScript;

inline std::tuple<int, bool> UnpackParamInfo(ParameterList* params) {
    return { params->GetParameterCount(), params->HasVariadicArgument() };
}

void SemanticAnalyzer::Walk(ParameterList* node) {
    for (auto* param : node->GetParameters()) {
        if (param->GetName() == "_") {
            param->SetSymbol(UnusedSymbol);
            return;
        }

        CheckRedefinition(param->GetName(), "parameter");
        auto* symbol = CreateLocalSymbol<VariableSymbol>(param->GetName(), VariableType::Parameter, /*readonly=*/false);

        param->SetSymbol(symbol);
    }
}

void SemanticAnalyzer::Walk(ScriptDeclaration* node) {
    AutoScope<ScriptScope> scope(this);

    m_global = scope.Scope.GetRootScope();

    UnusedSymbol = CreateGlobalSymbol<VariableSymbol>("<unused>", SourcePosition(), VariableType::Implicit, /*readonly=*/false);

    super::Walk(node);
}

void SemanticAnalyzer::Walk(ImportDeclaration* node) {
    if (!GetCurrentScope().GetDeclScope().CanImport()) {
        m_reporter.Report(node->GetImportPosition(), ReportID::SemaCannotImport);
    }
}

void SemanticAnalyzer::Walk(ExportDeclaration* node) {
    if (!GetCurrentScope().GetDeclScope().CanExport()) {
        m_reporter.Report(node->GetExportPosition(), ReportID::SemaCannotExport);
        return;
    }

    auto IsInvalidTarget = [](Symbol* symbol) {
        if (auto* var = symbol->As<VariableSymbol>()) {
            switch (var->GetVariableType()) {
                case VariableType::Parameter:
                case VariableType::Except:
                case VariableType::Implicit:
                    return true;

                default:
                    var->SetExported();
                    return false;
            }
        }

        return true;
    };

    // Check redefinition
    auto [result, _, symbol] = GetCurrentScope().Lookup(node->GetName());
    if (result == LookupResult::NotFound) {
        symbol = CreateLocalSymbol<VariableSymbol>(node->GetName(), VariableType::Global, /*readonly=*/false);
    }
    else {
        // export declaration with initializer is redefinition.
        if (node->HasValue()) {
            m_reporter.Report(node->GetName().GetPosition(), ReportID::SemaRedefinition, symbol->GetDescriptiveName())
                      .Reference(symbol->GetPosition());
        }
        else if (IsInvalidTarget(symbol)) {
            m_reporter.Report(node->GetName().GetPosition(), ReportID::SemaInvalidExportTarget,
                              symbol->GetDescriptiveName());
        }
    }

    node->SetSymbol(symbol);

    if (node->HasValue()) {
        super::Walk(node->GetValue());
        symbol->As<VariableSymbol>()->SetInitialized();
    }
}

void SemanticAnalyzer::Walk(FunctionDeclaration* node) {
    // Check redefinition
    auto [argc, vararg] = UnpackParamInfo(node->GetParameterList());

    CheckRedefinition(node->GetName(), argc, vararg, "function");

    node->SetSymbol(CreateGlobalSymbol<FunctionSymbol>(node->GetName(), argc, vararg));

    {
        AutoScope<FunctionScope> scope(this);
        super::Walk(node);
    }
}

void SemanticAnalyzer::Walk(ClassDeclaration* node) {
    CheckRedefinition(node->GetExtendName(), "class");

    // Check the class inherits itself.
    if (node->HasExtends() && node->GetName() == node->GetExtendName()) {
        m_reporter.Report(node->GetExtendName().GetPosition(), ReportID::SemaCannotExtendsSelf);
    }

    {
        AutoScope<ClassScope> scope(this);

        // Build symbols for members
        auto* symbol = BuildClassSymbol(node);
        GetGlobal().AddSymbol(symbol);

        super::Walk(node);
    }
}

void SemanticAnalyzer::Walk(ClassInitDeclaration* node) {
    AutoScope<MethodScope> scope(this, MethodScopeKind::Initializer);
    super::Walk(node);

    if (scope.Scope.HasInitializerCall()) {
        // Propagate to IR generator
        node->SetInitializerCall();
    }
}

void SemanticAnalyzer::Walk(ClassDeinitDeclaration* node) {
    AutoScope<MethodScope> scope(this, MethodScopeKind::Deinitializer);
    super::Walk(node);
}

void SemanticAnalyzer::Walk(ClassFieldDeclaration* node) {
    super::Walk(node);

    auto* field = node->GetSymbol()->As<FieldSymbol>();
    NEVER_BE_NULL(field);

    field->SetInitialized();
}

inline int GetRequiredArguments(OperatorKind kind) {
    static const int table[] = {
        -1, /* OperatorKind::Invalid */
        2,  /* OperatorKind::Add */
        2,  /* OperatorKind::Sub */
        2,  /* OperatorKind::Mul */
        2,  /* OperatorKind::Div */
        2,  /* OperatorKind::Mod */
        2,  /* OperatorKind::LeftShift */
        2,  /* OperatorKind::RightShift */
        2,  /* OperatorKind::BitAnd */
        2,  /* OperatorKind::BitOr */
        1,  /* OperatorKind::BitNot */
        1,  /* OperatorKind::InplaceAdd */
        1,  /* OperatorKind::InplaceSub */
        1,  /* OperatorKind::InplaceMul */
        1,  /* OperatorKind::InplaceDiv */
        1,  /* OperatorKind::InplaceMod */
        1,  /* OperatorKind::InplaceLeftShift */
        1,  /* OperatorKind::InplaceRightShift */
        1,  /* OperatorKind::InplaceBitAnd */
        1,  /* OperatorKind::InplaceBitOr */
        1,  /* OperatorKind::InplaceBitXor */
    };

    return table[static_cast<size_t>(kind)];
}

void SemanticAnalyzer::Walk(ClassMethodDeclaration* node) {
    auto GetKind = [](ClassMethodDeclaration* node) -> MethodScopeKind {
        if (node->IsOperator()) {
            constexpr auto range = Range(OperatorKind::InplaceAdd, OperatorKind::InplaceBitXor);
            return range.In(node->GetOperator()) ? MethodScopeKind::InplaceOperator : MethodScopeKind::Operator;
        }
        else {
            return node->IsStatic() ? MethodScopeKind::StaticMethod : MethodScopeKind::Method;
        }
    };

    // Check arguments but not on invalid operator.
    if (node->IsOperator() && node->GetOperator() != OperatorKind::Invalid) {
        auto requireArgc = GetRequiredArguments(node->GetOperator());
        auto [argc, vararg] = UnpackParamInfo(node->GetParameterList());

        if (requireArgc != argc) {
            m_reporter.Report(node->GetDefPosition(), ReportID::SemaArgumentMismatchInOperator,
                              requireArgc, OperatorKindToString(node->GetOperator()), argc);
        }

        if (vararg) {
            m_reporter.Report(node->GetParameterList()->GetEllipsisPosition(), ReportID::SemaVarArgInOperator);
        }
    }

    AutoScope<MethodScope> scope(this, GetKind(node));
    super::Walk(node);
}

void SemanticAnalyzer::Walk(ClassPropertyDeclaration* node) {
    auto kind = node->IsGetter() ? MethodScopeKind::Getter : MethodScopeKind::Setter;
    AutoScope<MethodScope> scope(this, kind);

    // Add implicit parameters
    if (node->IsSubscript()) {
        CreateLocalSymbol<VariableSymbol>(VariableSymbol::Index, SourcePosition(), VariableType::Implicit,
            /*readonly=*/false);
    }

    if (node->IsSetter()) {
        CreateLocalSymbol<VariableSymbol>(VariableSymbol::Value, SourcePosition(), VariableType::Implicit,
            /*readonly=*/false);
    }

    super::Walk(node);
}

void SemanticAnalyzer::Walk(TaskDeclaration* node) {
    CheckRedefinition(node->GetName(), "task");

    // Check the task extends itself.
    if (node->HasExtends() && node->GetName() == node->GetExtendName()) {
        m_reporter.Report(node->GetExtendName().GetPosition(), ReportID::SemaCannotExtendsSelf);
    }

    // Check the task depends on itself.
    for (auto& dep : node->GetDependencyNames()) {
        if (dep == node->GetName()) {
            m_reporter.Report(dep.GetPosition(), ReportID::SemaCannotDependOnSelf);
        }
    }

    {
        AutoScope<ClassScope> scope(this);

        auto* symbol = BuildTaskSymbol(node);
        GetGlobal().AddSymbol(symbol);

        super::Walk(node);
    }
}

void SemanticAnalyzer::Walk(VariableDeclaration* node) {
    if (node->GetName() == "_") {
        m_reporter.Report(node->GetName().GetPosition(), ReportID::SemaReservedUnderscore);
        return;
    }

    CheckRedefinition(node->GetName(), "variable");

    auto type = GetCurrentScope().IsGlobal() ? VariableType::Global : VariableType::Local;
    auto* var = CreateLocalSymbol<VariableSymbol>(node->GetName(), type, node->IsConst());
    node->SetSymbol(var);

    super::Walk(node->GetValue());

    // Set variable initialized.
    var->SetInitialized();
}

void SemanticAnalyzer::Walk(TaskActionDeclaration* node) {
    AutoScope<MethodScope> scope(this, MethodScopeKind::Action);

    // add symbols
    static const std::string_view symbols[] = {
        VariableSymbol::Inputs,
        VariableSymbol::Outputs,
        VariableSymbol::Input,
        VariableSymbol::Output,
    };

    auto* method = node->GetSymbol()->As<MethodSymbol>();
    NEVER_BE_NULL(method);

    for (auto i = 0; i < method->GetArgumentCount(); i++) {
        CreateLocalSymbol<VariableSymbol>(symbols[i], SourcePosition(), VariableType::Implicit, /*readonly=*/false);
    }

    super::Walk(node->GetBody());
}

void SemanticAnalyzer::Walk(TaskPropertyDeclaration* node) {
    super::Walk(node->GetValue());

    auto* field = node->GetSymbol()->As<FieldSymbol>();
    NEVER_BE_NULL(field);

    field->SetInitialized();
}