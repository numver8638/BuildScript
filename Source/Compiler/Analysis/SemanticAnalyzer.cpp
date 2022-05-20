/*
 * SemanticAnalyzer.cpp
 * - Verify and analyze AST given by Parser.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.h>

#include <unordered_set>

#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/ParameterList.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>
#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.Builder.h>
#include <BuildScript/Compiler/Analysis/Scope.h>

using namespace BuildScript;

inline std::tuple<int, bool> UnpackParamInfo(ParameterList* params) {
    return { params->GetParameterCount(), params->HasVariadicArgument() };
}

SemanticAnalyzer::SemanticAnalyzer(Context& context)
    : m_reporter(context.GetReporter()), m_context(context) {}

Symbol* SemanticAnalyzer::FindTypeSymbol(const Identifier& name) {
    auto [result, _, symbol] = GetCurrentScope().LookupLocal(name);
    if (result == LookupResult::NotFound) {
        return CreateGlobalSymbol<TypeSymbol>(name);
    }

    switch (symbol->GetType()) {
        case SymbolType::Type:
        case SymbolType::Class:
        case SymbolType::Task:
            return symbol;

        default:
            m_reporter.Report(name.GetPosition(), ReportID::SemaNotAType, name.GetString());
            return nullptr;
    }
}

void SemanticAnalyzer::CheckRedefinition(const Identifier& name, std::string_view expected) {
    auto [result, _, symbol] = GetCurrentScope().LookupLocal(name);

    if (result == LookupResult::NotFound) { return; }

    if (auto* undeclared = symbol->As<UndeclaredSymbol>()) {
        m_reporter.Report(undeclared->GetPosition(), ReportID::SemaUsedBeforeDeclare, expected, name.GetString())
                  .Reference(name.GetPosition());
    }
    else {
        m_reporter.Report(name.GetPosition(), ReportID::SemaRedefinition, symbol->GetDescriptiveName())
                  .Reference(symbol->GetPosition());
    }
}

void SemanticAnalyzer::CheckRedefinition(const Identifier& name, int argc, bool vararg, std::string_view expected) {
    auto [result, _, symbol] = GetCurrentScope().LookupLocal(name, argc, vararg);

    if (result == LookupResult::FoundInScope) {
        if (auto* undeclared = symbol->As<UndeclaredSymbol>()) {
            m_reporter.Report(undeclared->GetPosition(), ReportID::SemaUsedBeforeDeclare, expected, name.GetString())
                      .Reference(name.GetPosition());
        }
        else {
            m_reporter.Report(name.GetPosition(), ReportID::SemaRedefinition, symbol->GetDescriptiveName())
                      .Reference(symbol->GetPosition());
        }
    }
    else if (result == LookupResult::FoundRedefinition) {
        m_reporter.Report(name.GetPosition(), ReportID::SemaRedeclaration, symbol->GetDescriptiveName(), expected)
                  .Reference(symbol->GetPosition());
    }
}

Symbol* SemanticAnalyzer::BuildClassSymbol(ClassDeclaration* node) {
    auto* owner = new (m_context.GetAllocator()) ClassSymbol(node->GetName());

    for (auto* member : node->GetMembers()) {
        switch (member->GetKind()) {
            case DeclarationKind::Invalid:
                // Do nothing.
                break;

            case DeclarationKind::ClassInit: {
                auto* init = member->As<ClassInitDeclaration>();
                NEVER_BE_NULL(init);

                auto [argc, vararg] = UnpackParamInfo(init->GetParameterList());
                CheckRedefinition(init->GetName(), argc, vararg, "initializer");

                init->SetSymbol(
                    CreateLocalSymbol<MethodSymbol>(
                        MethodSymbol::InitializerName, init->GetInitPosition(), argc, vararg, /*isStatic=*/false, owner
                    )
                );
                break;
            }

            case DeclarationKind::ClassDeinit: {
                auto* deinit = member->As<ClassDeinitDeclaration>();
                NEVER_BE_NULL(deinit);

                CheckRedefinition(deinit->GetName(), 0, false, "deinitializer");

                deinit->SetSymbol(
                    CreateLocalSymbol<MethodSymbol>(
                        MethodSymbol::DeinitializerName, deinit->GetDeinitPosition(), 0, false, /*isStatic=*/false, owner
                    )
                );
                break;
            }

            case DeclarationKind::ClassField: {
                auto* field = member->As<ClassFieldDeclaration>();
                NEVER_BE_NULL(field);

                CheckRedefinition(field->GetName(), "field");
                field->SetSymbol(CreateLocalSymbol<FieldSymbol>(field->GetName(), field->GetAccessFlag(), owner));
                break;
            }

            case DeclarationKind::ClassMethod: {
                auto* method = member->As<ClassMethodDeclaration>();
                NEVER_BE_NULL(method);

                auto [argc, vararg] = UnpackParamInfo(method->GetParameterList());
                CheckRedefinition(method->GetName(), argc, vararg, "method");

                method->SetSymbol(
                    CreateLocalSymbol<MethodSymbol>(method->GetName(), argc, vararg, method->IsStatic(), owner)
                );
                break;
            }

            case DeclarationKind::ClassProperty: {
                auto* prop = member->As<ClassPropertyDeclaration>();
                NEVER_BE_NULL(prop);

                MethodSymbol* method = nullptr;
                auto [result, _, symbol] = GetCurrentScope().LookupLocal(prop->GetName());
                if (result == LookupResult::FoundInScope) {
                    auto* propSymbol = symbol->As<PropertySymbol>();

                    if (propSymbol == nullptr) {
                        // redeclaration
                        m_reporter.Report(prop->GetName().GetPosition(), ReportID::SemaRedeclaration,
                                          symbol->GetDescriptiveName(), "property");
                        break;
                    }

                    if ((prop->IsGetter() && propSymbol->HasGetter())
                     || (prop->IsSetter() && propSymbol->HasSetter())) {
                        // redefinition
                        m_reporter.Report(prop->GetName().GetPosition(), ReportID::SemaRedefinition,
                                          symbol->GetDescriptiveName());
                        break;
                    }
                }
                else /* (result == LookupResult::NotFound) */ {
                    symbol = CreateLocalSymbol<PropertySymbol>(prop->GetName(), owner);
                }

                auto argc = (prop->IsGetter() ? 0 : 1);
                argc += (prop->IsSubscript() ? 1 : 0);

                method = new (m_context.GetAllocator()) MethodSymbol(prop->GetName(), argc, false, false, owner);

                prop->SetSymbol(symbol);
                prop->SetMethodSymbol(method);

                if (prop->IsGetter()) {
                    symbol->As<PropertySymbol>()->SetGetter(method);
                }
                else {
                    symbol->As<PropertySymbol>()->SetSetter(method);
                }

                break;
            }

            default:
                NOT_REACHABLE;
                break;
        }
    }

    node->SetSymbol(owner);
    return owner;
}

inline std::tuple<std::string_view, int> GetActionNameAndArgc(TaskActionDeclaration* action) {
    std::string_view name;
    int argc;
    switch (action->GetActionKind()) {
        case ActionKind::Do:
            name = Symbol::DoClauseName;
            argc = 4;
            break;

        case ActionKind::DoFirst:
            name = Symbol::DoFirstClauseName;
            argc = 2;
            break;

        case ActionKind::DoLast:
            name = Symbol::DoLastClauseName;
            argc = 2;
            break;
    }

    return { name, argc };
}

Symbol* SemanticAnalyzer::BuildTaskSymbol(TaskDeclaration* node) {
    auto* owner = new (m_context.GetAllocator()) TaskSymbol(node->GetName());

    SourcePosition inputsPos, outputsPos;

    for (auto* member : node->GetMembers()) {
        switch (member->GetKind()) {
            case DeclarationKind::Invalid:
                // Do nothing.
                break;

            case DeclarationKind::TaskInputs: {
                auto* inputs = member->As<TaskInputsDeclaration>();
                NEVER_BE_NULL(inputs);

                if (inputsPos) {
                    m_reporter.Report(inputs->GetInputsPosition(), ReportID::SemaDuplicatedInputs)
                              .Reference(inputsPos);
                }
                else {
                    inputsPos = inputs->GetInputsPosition();
                }
                break;
            }

            case DeclarationKind::TaskOutputs: {
                auto* outputs = member->As<TaskOutputsDeclaration>();
                NEVER_BE_NULL(outputs);

                if (outputsPos) {
                    m_reporter.Report(outputs->GetOutputsPosition(), ReportID::SemaDuplicatedOutputs)
                              .Reference(outputsPos);
                }
                else {
                    outputsPos = outputs->GetOutputsPosition();
                }
                break;
            }

            case DeclarationKind::TaskAction: {
                auto* action = member->As<TaskActionDeclaration>();
                NEVER_BE_NULL(action);

                // Check action clause already declared.
                auto [name, argc] = GetActionNameAndArgc(action);
                auto [result, _, symbol] = GetCurrentScope().LookupLocal(name, argc, /*vararg=*/false);

                if (result == LookupResult::FoundInScope) {
                    m_reporter.Report(action->GetKeywordPosition(), ReportID::SemaRedefinition, "action")
                              .Reference(symbol->GetPosition());
                }

                action->SetSymbol(
                    CreateLocalSymbol<MethodSymbol>(
                        name, action->GetKeywordPosition(), argc, /*vararg=*/false, /*static=*/false, owner
                    )
                );

                break;
            }

            case DeclarationKind::TaskProperty: {
                auto* prop = member->As<TaskPropertyDeclaration>();
                NEVER_BE_NULL(prop);

                static const std::unordered_set<std::string_view> ReservedIdentifiers = {
                    VariableSymbol::Inputs,
                    VariableSymbol::Input,
                    VariableSymbol::Outputs,
                    VariableSymbol::Output,
                    Symbol::DoClauseName,
                    Symbol::DoFirstClauseName,
                    Symbol::DoLastClauseName
                };

                auto pos = prop->GetName().GetPosition();
                auto name = prop->GetName().GetString();
                if (ReservedIdentifiers.find(name) != ReservedIdentifiers.end()) {
                    m_reporter.Report(pos, ReportID::SemaReservedIdentifier, name);
                }
                else {
                    CheckRedefinition(prop->GetName(), "property");
                    prop->SetSymbol(
                        CreateLocalSymbol<FieldSymbol>(prop->GetName(), AccessFlags::Static, owner)
                    );
                }
                break;
            }

            default:
                NOT_REACHABLE;
                break;
        }
    }

    node->SetSymbol(owner);
    return owner;
}