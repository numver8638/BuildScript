/*
 * SemanticAnalyzer.Statement.cpp
 * - Verify and analyze AST given by Parser.
 *   This file contains part of SemanticAnalyzer that check statements.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.h>

#include <unordered_map>

#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.AutoScope.h>
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.Builder.h>

#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>

using namespace BuildScript;

void SemanticAnalyzer::Walk(BlockStatement* node) {
    AutoScope<LocalScope> scope(this, LocalScopeKind::Block);

    for (auto* n : node->GetNodes()) {
        super::Walk(n);

        // To check validity of pass statement and initializer invocation, nodes should be counted.
        GetCurrentScope().CountNode();
    }
}

void SemanticAnalyzer::Walk(MatchStatement* node) {
    super::Walk(node);

    // Check label collision
    Label* _default = nullptr;
    std::unordered_map<Value, Label*> cases;

    for (auto* stmt : node->GetStatements()) {
        auto* labeled = stmt->As<LabeledStatement>();
        NEVER_BE_NULL(labeled);

        for (auto* label : labeled->GetLabels()) {
            if (label->IsDefault()) {
                if (_default != nullptr) {
                    m_reporter.Report(label->GetDefaultPosition(), ReportID::SemaDuplicatedDefault)
                              .Reference(label->GetDefaultPosition());
                }
                else { _default = label; }
            }
            // Check only if case value is valid.
            else if (label->GetEvaluatedCaseValue()) {
                auto [it, inserted] = cases.emplace(label->GetEvaluatedCaseValue(), label);

                if (!inserted) {
                    m_reporter.Report(label->GetCasePosition(), ReportID::SemaDuplicatedCase)
                              .Reference(it->second->GetCasePosition());
                }
            }
        }
    }

    if (_default == nullptr) {
        m_reporter.Report(node->GetMatchPosition(), ReportID::SemaNoDefault);
    }
}

void SemanticAnalyzer::Walk(LabeledStatement* node) {
    for (auto* label : node->GetLabels()) {
        if (label->IsDefault()) { continue; }

        // TODO: Allow compile-time expression in case value.
        if (auto* literal = label->GetCaseValue()->As<LiteralExpression>()) {
            switch (literal->GetLiteralType()) {
                case LiteralType::String:
                    // interpolated string is not allowed.
                    if (literal->HasInterpolations()) {
                        m_reporter.Report(label->GetCasePosition(), ReportID::SemaInvalidCaseValue);
                        m_reporter.Report(literal->GetPosition(), ReportID::SemaInterpolatedStringInLabel);
                        break;
                    }
                    [[fallthrough]];

                default:
                    label->SetEvaluatedCaseValue(literal->GetRawValue());
                    break;

                case LiteralType::Variable:
                    m_reporter.Report(label->GetCasePosition(), ReportID::SemaInvalidCaseValue);
                    break;

                case LiteralType::Self:
                    m_reporter.Report(label->GetCasePosition(), ReportID::SemaInvalidCaseValue);
                    m_reporter.Report(literal->GetPosition(), ReportID::SemaSelfInLabel);
                    break;

                case LiteralType::Super:
                    m_reporter.Report(label->GetCasePosition(), ReportID::SemaInvalidCaseValue);
                    m_reporter.Report(literal->GetPosition(), ReportID::SemaSuperInLabel);
                    break;
            }
        }
        else {
            m_reporter.Report(label->GetCasePosition(), ReportID::SemaInvalidCaseValue);
            m_reporter.Report(label->GetCasePosition(), ReportID::SemaConstantExpressionInLabel);
        }
    }

    {
        AutoScope<LocalScope> scope(this, LocalScopeKind::Match);

        for (auto* stmt: node->GetNodes()) {
            super::Walk(stmt);

            // To check validity of pass statement, nodes should be counted.
            GetCurrentScope().CountNode();
        }
    }
}

void SemanticAnalyzer::Walk(ForStatement* node) {
    super::Walk(node->GetExpression());

    {
        AutoScope<LocalScope> scope(this, LocalScopeKind::Loop);

        if (node->GetParameter()->GetName() != "_") {
            auto* var = CreateLocalSymbol<VariableSymbol>(node->GetParameter()->GetName(), VariableType::Local,
                /*readonly=*/false);
            node->GetParameter()->SetSymbol(var);
        }

        super::Walk(node->GetBody());
    }
}

void SemanticAnalyzer::Walk(WhileStatement* node) {
    super::Walk(node->GetCondition());

    {
        AutoScope<LocalScope> scope(this, LocalScopeKind::Loop);
        super::Walk(node->GetBody());
    }
}

void SemanticAnalyzer::Walk(WithStatement* node) {
    super::Walk(node->GetExpression());

    {
        AutoScope<LocalScope> scope(this, LocalScopeKind::Block);

        if (node->HasCapture()) {
            auto* capture = node->GetCapture();
            if (capture->GetName() == "_") {
                m_reporter.Report(capture->GetName().GetPosition(), ReportID::SemaSimpilfyCapture)
                          .Remove(SourceRange::Merge(node->GetAsPosition(), capture->GetName().GetRange()));
                capture->SetSymbol(UnusedSymbol);
            }
            else {
                capture->SetSymbol(
                    CreateLocalSymbol<VariableSymbol>(capture->GetName(), VariableType::Local, /*readonly=*/true)
                );
            }
        }

        super::Walk(node->GetBody());
    }
}

void SemanticAnalyzer::Walk(TryStatement* node) {
    super::Walk(node);

    // check duplicated handler
    std::unordered_map<Symbol*, ExceptStatement*> map;

    for (auto* handler : node->GetHandlers()) {
        if (auto* except = handler->As<ExceptStatement>()) {
            auto it = map.find(except->GetTypeSymbol());
            if (it != map.end()) {
                m_reporter.Report(except->GetExceptPosition(), ReportID::SemaDuplicatedExcept)
                          .Reference(it->second->GetExceptPosition());
            }
        }
    }
}

void SemanticAnalyzer::Walk(ExceptStatement* node) {
    node->SetTypeSymbol(FindTypeSymbol(node->GetTypename()));

    {
        AutoScope<LocalScope> scope(this, LocalScopeKind::Except);

        if (node->HasCapture()) {
            auto* capture = node->GetCapture();
            if (capture->GetName() == "_") {
                m_reporter.Report(capture->GetName().GetPosition(), ReportID::SemaSimpilfyCapture)
                          .Remove(SourceRange::Merge(node->GetAsPosition(), capture->GetName().GetRange()));
                capture->SetSymbol(UnusedSymbol);
            }
            else {
                auto* var = CreateLocalSymbol<VariableSymbol>(capture->GetName(), VariableType::Except,
                                                              /*readonly=*/true);
                capture->SetSymbol(var);
            }
        }

        super::Walk(node);
    }
}

void SemanticAnalyzer::Walk(FinallyStatement* node) {
    // return, break, continue are not allowed in finally statement.
    AutoScope<LocalScope> scope(this, LocalScopeKind::Finally);

    super::Walk(node);
}

void SemanticAnalyzer::Walk(BreakStatement* node) {
    super::Walk(node);

    if (!GetCurrentScope().CanBreak()) {
        auto id = GetCurrentScope().InFinally() ? ReportID::SemaCannotBreakInFinally : ReportID::SemaCannotBreak;
        m_reporter.Report(node->GetBreakPosition(), id);
    }
}

void SemanticAnalyzer::Walk(ContinueStatement* node) {
    super::Walk(node);

    if (!GetCurrentScope().CanContinue()) {
        auto id = GetCurrentScope().InFinally() ? ReportID::SemaCannotContinueInFinally : ReportID::SemaCannotContinue;
        m_reporter.Report(node->GetContinuePosition(), id);
    }
}

void SemanticAnalyzer::Walk(ReturnStatement* node) {
    super::Walk(node);

    auto pos = node->GetReturnPosition();

    if (GetCurrentScope().CanReturn()) {
        if (node->HasReturnValue() && GetCurrentScope().GetDeclScope().GetReturnFlag() == ReturnFlags::NeverReturn) {
            m_reporter.Report(pos, ReportID::SemaMustReturnValue);
            m_reporter.Report(pos, ReportID::SemaReturnNoneIfNeeded);
        }

        if (!node->HasReturnValue() && GetCurrentScope().GetDeclScope().GetReturnFlag() == ReturnFlags::MustReturn) {
            m_reporter.Report(pos, ReportID::SemaNeverReturnValue);
        }
    }
    else {
        auto id = GetCurrentScope().InFinally() ? ReportID::SemaCannotReturnInFinally : ReportID::SemaCannotReturn;
        m_reporter.Report(pos, id);
    }
}

void SemanticAnalyzer::Walk(PassStatement* node) {
    if (GetCurrentScope().IsGlobal() || !GetCurrentScope().IsFirstNodeOfScope()) {
        m_reporter.Report(node->GetPassPosition(), ReportID::SemaMisusePass);
        m_reporter.Report(node->GetPassPosition(), GetCurrentScope().IsGlobal() ? ReportID::SemaCannotUsePassInGlobal
                                                                                : ReportID::SemaPassUsage);
    }
}

void SemanticAnalyzer::Walk(AssignStatement* node) {
    super::Walk(node);

    switch (node->GetTarget()->GetKind()) {
        case ExpressionKind::MemberAccess:
        case ExpressionKind::Subscript:
            // Do nothing. Invalidity of operation is evaluated in runtime.
            break;

        case ExpressionKind::Literal: {
            auto* literal = node->GetTarget()->As<LiteralExpression>();
            NEVER_BE_NULL(literal);

            if (literal->GetLiteralType() == LiteralType::Variable) {
                auto* symbol = literal->GetSymbol();
                NEVER_BE_NULL(symbol);

                if (symbol->IsWritable() == Trilean::False) {
                    m_reporter.Report(node->GetOpPosition(), ReportID::SemaCannotAssign, symbol->GetDescriptiveName());
                }

                auto* scope = GetCurrentScope().GetDeclScope().As<ClosureScope>();
                if (scope != nullptr && scope->IsBoundedLocal(symbol)) {
                    m_reporter.Report(node->GetOpPosition(), ReportID::SemaCannotAssignToBoundedVariable,
                                      symbol->GetName());
                }

                break;
            }
            [[fallthrough]];
        }

        default:
            m_reporter.Report(node->GetOpPosition(), ReportID::SemaCannotAssignToValue);
            break;
    }
}