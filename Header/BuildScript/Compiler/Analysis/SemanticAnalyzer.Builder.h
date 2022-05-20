/*
 * SemanticAnalyzer.Builder.h
 * - .
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_BUILDER_H
#define BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_BUILDER_H

#include <BuildScript/Compiler/Analysis/SemanticAnalyzer.h>

#include <BuildScript/Compiler/Analysis/Scope.h>

namespace BuildScript {
    template <typename SymbolType, typename... ArgsType>
    SymbolType* SemanticAnalyzer::CreateLocalSymbol(ArgsType&& ...args) {
        auto* symbol = new (m_context.GetAllocator()) SymbolType(std::forward<ArgsType&&>(args)...);

        GetCurrentScope().AddSymbol(symbol);

        return symbol;
    }

    template <typename SymbolType, typename... ArgsType>
    SymbolType* SemanticAnalyzer::CreateGlobalSymbol(ArgsType&& ...args) {
        auto* symbol = new (m_context.GetAllocator()) SymbolType(std::forward<ArgsType&&>(args)...);

        GetGlobal().AddSymbol(symbol);

        return symbol;
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_BUILDER_H