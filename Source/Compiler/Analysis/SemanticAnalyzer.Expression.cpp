/*
 * SemanticAnalyzer.Expression.cpp
 * - Verify and analyze AST given by Parser.
 *   This file contains part of SemanticAnalyzer that check expressions.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.h>

#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.AutoScope.h>
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.Builder.h>

#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>
#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/ErrorReporter.h>

using namespace BuildScript;

void SemanticAnalyzer::Walk(TypeTestExpression* node) {
    node->SetTypeSymbol(FindTypeSymbol(node->GetTypename()));

    super::Walk(node->GetTarget());
}

void SemanticAnalyzer::Walk(InvocationExpression* node) {
    super::Walk(node);

    // Check the invocation is initializer call
    auto isInitCall = false;
    SourcePosition pos;
    auto IsSelfOrSuper = [](LiteralExpression* expr) -> bool {
        return expr->GetLiteralType() == LiteralType::Self || expr->GetLiteralType() == LiteralType::Super;
    };

    if (auto* literal = node->GetTarget()->As<LiteralExpression>()) {
        isInitCall = IsSelfOrSuper(literal);
        pos = literal->GetPosition();
    }

    if (!isInitCall) { return; }

    auto* scope = GetCurrentScope().GetDeclScope().As<MethodScope>();
    if (scope == nullptr || scope->GetMethodKind() != MethodScopeKind::Initializer) {
        // invalid call
        m_reporter.Report(node->GetOpenParenPosition(), ReportID::SemaInvalidInitCall);
    }
    else if (scope->HasInitializerCall()) {
        // already called.
        m_reporter.Report(node->GetOpenParenPosition(), ReportID::SemaDuplicatedInitCall);
    }
    else if (!GetCurrentScope().IsRootOfDeclScope() || !GetCurrentScope().IsFirstNodeOfScope()) {
        // must be first statement in block.
        m_reporter.Report(node->GetOpenParenPosition(), ReportID::SemaInitCallBeforeStatement);
    }
    else {
        // valid
        scope->SetInitializerCall();
    }
}

void SemanticAnalyzer::Walk(ClosureExpression* node) {
    node->SetSymbol(CreateGlobalSymbol<ClosureSymbol>(node->GetArrowPosition()));
    {
        AutoScope<ClosureScope> scope(this);
        super::Walk(node);

        node->SetBoundedLocals(scope.Scope.GetBoundedLocals());
    }
}

void SemanticAnalyzer::Walk(LiteralExpression* node) {
    super::Walk(node);

    auto IsLocalSymbol = [](Symbol* symbol) {
        auto* var = symbol->As<VariableSymbol>();

        if (var == nullptr) { return false; }

        switch (var->GetVariableType()) {
            case VariableType::Global:
            case VariableType::Exported:
                return false;

            default:
                return true;
        }
    };
    auto IsClassMember = [](Symbol* symbol) {
        if (auto* method = symbol->As<MethodSymbol>()) { return !method->IsStatic(); }

        if (auto* property = symbol->As<PropertySymbol>()) { return true; }

        return false;
    };
    Symbol* symbol;
    int64_t depth;

    switch (node->GetLiteralType()) {
        default:
            // do nothing.
            return;

        case LiteralType::Variable: {
            if (node->AsString() == "_") {
                m_reporter.Report(node->GetPosition(), ReportID::SemaReservedUnderscore);
                return;
            }

            auto [R, D, S] = GetCurrentScope().Lookup(node->AsString());

            if (R == LookupResult::NotFound) {
                S = CreateGlobalSymbol<UndeclaredSymbol>(node->AsString(), node->GetPosition());
            }
            else if (S->IsInitialized() == Trilean::False) {
                m_reporter.Report(node->GetPosition(), ReportID::SemaCannotUseBeforeInit, S->GetDescriptiveName());
            }

            symbol = S;
            depth = D;
            break;
        }

        case LiteralType::Self:
        case LiteralType::Super:
            if (!GetCurrentScope().GetDeclScope().InInstance()) {
                auto id = (node->GetLiteralType() == LiteralType::Self) ? ReportID::SemaCannotUseSelfOutOfClass
                                                                        : ReportID::SemaCannotUseSuperOutOfClass;
                m_reporter.Report(node->GetPosition(), id);
            }

            symbol = (node->GetLiteralType() == LiteralType::Self) ? VariableSymbol::GetSelf()
                                                                   : VariableSymbol::GetSuper();
            depth = GetCurrentScope().GetScopeDepth();
            break;
    }

    auto* scope = GetCurrentScope().GetDeclScope().As<ClosureScope>();
    if (scope != nullptr && depth < scope->GetRootScope()->GetScopeDepth()) {
        if (IsLocalSymbol(symbol)) {
            scope->AddBoundedLocal(symbol);

            // translate normal symbol to BoundedLocalSymbol
            symbol = new (m_context.GetAllocator()) BoundedLocalSymbol(symbol);
        }

        if (IsClassMember(symbol)) {
            scope->AddBoundedLocal(VariableSymbol::GetSelf());
        }
    }

    node->SetSymbol(symbol);
}