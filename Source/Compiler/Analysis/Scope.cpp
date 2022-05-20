/*
 * Scope.cpp
 * - Trace scopes of source and symbols declared in the scope.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Analysis/Scope.h>

#include <BuildScript/Compiler/Symbol/Symbol.h>

using namespace BuildScript;

LocalScope::Result LocalScope::LookupLocal(std::string_view name) const {
    auto it = super::find(name);

    if (it != super::end()) {
        return { LookupResult::FoundInScope, m_depth, it->second };
    }

    return { LookupResult::NotFound, -1, nullptr };
}

LocalScope::Result LocalScope::LookupLocal(std::string_view name, int argc, bool vararg) const {
    auto it = super::find(name);

    while (it != super::end()) {
        auto [_, symbol] = *it++;

        if (auto* func = symbol->As<FunctionSymbol>()) {
            if (func->GetArgumentCount() != argc || func->HasVariadicArgument() != vararg) { continue; }
        }
        else if (auto* method = symbol->As<MethodSymbol>()) {
            if (method->GetArgumentCount() != argc || method->HasVariadicArgument() != vararg) { continue; }
        }
        else {
            return { LookupResult::FoundRedefinition, m_depth, symbol };
        }

        return { LookupResult::FoundInScope, m_depth, symbol };
    }

    return { LookupResult::NotFound, -1, nullptr };
}

LocalScope::Result LocalScope::Lookup(std::string_view name) const {
    auto* self = this;
    while (self != nullptr) {
        auto [result, depth, symbol] = self->LookupLocal(name);
        if (result != LookupResult::NotFound) {
            return { (self == this) ? LookupResult::FoundInScope : LookupResult::FoundOutOfScope, depth, symbol };
        }

        self = self->GetParent();
    }

    return { LookupResult::NotFound, -1, nullptr };
}

void LocalScope::AddSymbol(Symbol* symbol) {
    super::insert({ symbol->GetName(), symbol });
}