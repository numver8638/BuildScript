/*
 * SymbolTable.cpp
 * - Container for lookup and store symbols.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Symbol/SymbolTable.h>

#include <BuildScript/Compiler/Symbol/Symbol.h>

using namespace BuildScript;

SymbolTable::Result SymbolTable::LookupLocal(std::string_view name, int argc, bool vararg) const  {
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
            return { LookupResult::FoundRedefinition, symbol };
        }

        return { LookupResult::FoundInScope, symbol };
    }

    return { LookupResult::NotFound, nullptr };
}

void SymbolTable::AddSymbol(Symbol* symbol) {
    super::insert({ symbol->GetName(), symbol });
}