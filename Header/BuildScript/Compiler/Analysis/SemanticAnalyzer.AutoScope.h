/*
 * SemanticAnalyzer.AutoScope.h
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_AUTOSCOPE_H
#define BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_AUTOSCOPE_H

#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.h>
#include <BuildScript/Compiler/Analysis/Scope.h>

namespace BuildScript {
    template <typename ScopeType>
    struct AutoScope {
        SemanticAnalyzer* Self;
        ScopeType Scope;

        template <typename... ArgsType>
        explicit AutoScope(SemanticAnalyzer* self, ArgsType&&... args)
            : Self(self), Scope(self->m_scope, std::forward<ArgsType&&>(args)...) {
            Self->m_scope = Scope.GetRootScope();
        }

        ~AutoScope() noexcept {
            Self->m_scope = Scope.GetRootScope()->GetParent();
        }
    }; // end struct AutoScope<>

    template <>
    struct AutoScope<LocalScope> {
        SemanticAnalyzer* Self;
        LocalScope Scope;

        AutoScope(SemanticAnalyzer* self, LocalScopeKind kind)
            : Self(self), Scope(self->m_scope, kind, self->m_scope->GetDeclScope()) {
            Self->m_scope = &Scope;
        }

        ~AutoScope() noexcept {
            Self->m_scope = Scope.GetParent();
        }
    }; // end struct AutoScope<LocalScope>
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_AUTOSCOPE_H