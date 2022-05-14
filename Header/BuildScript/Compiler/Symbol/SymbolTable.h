/*
 * SymbolTable.h
 * - Container for lookup and store symbols.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_SYMBOL_SYMBOLTABLE_H
#define BUILDSCRIPT_COMPILER_SYMBOL_SYMBOLTABLE_H

#include <string_view>
#include <unordered_map>

#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Compiler/ManagedObject.h>

namespace BuildScript {
    class Symbol; // Defined in <BuildScript/Compiler/Symbol/Symbol.h>

    /**
     * @brief Represents result of lookup.
     */
    enum class LookupResult {
        NotFound,           //!< Not found in any table.
        FoundInScope,       //!< Found a symbol in the table.
        FoundOutOfScope,    //!< Found a symbol in parent's table.
        FoundRedefinition,  //!< Found a symbol but the symbol has different signature.
    }; // end enum LookupResult

    /**
     * @brief Container for lookup and store symbols.
     */
    class SymbolTable final : public ManagedObject,
                              private std::unordered_multimap<std::string_view, Symbol*> {
        using super = std::unordered_multimap<std::string_view, Symbol*>;

    private:
        SymbolTable* m_parent;

    public:
        explicit SymbolTable(SymbolTable* parent = nullptr)
            : m_parent(parent) {}

        /**
         * @brief Represents result of lookup. Paired with @c LookupResult and found @c Symbol.
         */
        using Result = std::tuple<LookupResult, Symbol*>;

        using super::begin;
        using super::end;

        /**
         * @brief Get a parent of the table.
         * @return a @c SymbolTable that represents the parent of the table.
         *         May return @c nullptr if the table does not have parent.
         */
        SymbolTable* GetParent() const { return m_parent; }

        /**
         * @brief Search a symbol with given name only in current table.
         * @param name a name of a symbol to find.
         * @return a @c Result that represents result of lookup.
         * @see Result
         */
        Result LookupLocal(std::string_view name) const {
            auto it = super::find(name);

            if (it != super::end()) {
                return { LookupResult::FoundInScope, it->second };
            }

            return { LookupResult::NotFound, nullptr };
        }

        /**
         * @copydoc LookupLocal(std::string_view) const
         */
        Result LookupLocal(const Identifier& name) const { return LookupLocal(name.GetString()); }

        /**
         * @brief Search a symbol with given name.
         * @param name a name of a symbol to find.
         * @return a @c Result that represents result of lookup.
         * @see Result
         */
        Result Lookup(std::string_view name) const {
            auto* self = this;
            while (self != nullptr) {
                auto [result, symbol] = self->LookupLocal(name);
                if (result != LookupResult::NotFound) {
                    return { (self == this) ? LookupResult::FoundInScope : LookupResult::FoundOutOfScope, symbol };
                }

                self = self->GetParent();
            }

            return { LookupResult::NotFound, nullptr };
        }

        /**
         * @copydoc Lookup(std::string_view) const
         */
        Result Lookup(const Identifier& name) const { return Lookup(name.GetString()); }

        /**
         * @brief Search a symbol with given name, argument count and variadic argument.
         * @param name a name of a symbol to find.
         * @param argc a count of argument to find.
         * @param vararg a flag that represents a function/method has variadic argument to find.
         * @return a @c Result that represents result of lookup.
         * @see Result
         */
        Result LookupLocal(std::string_view name, int argc, bool vararg) const;

        /**
         * @copydoc LookupLocal(std::string_view, int, bool) const
         */
        Result LookupLocal(const Identifier& name, int argc, bool vararg) const {
            return LookupLocal(name.GetString(), argc, vararg);
        }

        /**
         * @brief Add a symbol into the table.
         * @param symbol a @c Symbol to add into the table.
         */
        void AddSymbol(Symbol* symbol);
    }; // end class SymbolTable
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_SYMBOL_SYMBOLTABLE_H