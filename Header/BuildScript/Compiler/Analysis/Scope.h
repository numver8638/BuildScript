/*
 * Scope.h
 * - Trace scopes of source and symbols declared in the scope.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ANALYSIS_SCOPE_H
#define BUILDSCRIPT_COMPILER_ANALYSIS_SCOPE_H

#include <string_view>
#include <tuple>
#include <unordered_map>
#include <set>
#include <utility>
#include <vector>

#include <BuildScript/Assert.h>
#include <BuildScript/Compiler/Identifier.h>

namespace BuildScript {
    class DeclScope;
    class Symbol;

    /**
     * @brief
     */
    enum class LocalScopeKind {
        Root,       //!< The scope is ...
        Block,      //!<
        Loop,       //!< The scope is in the loop(for, while).
        Match,      //!< The scope is in the match statement.
        Except,     //!< The scope is in the except statement.
        Finally     //!< The scope is in the finally statement.
    }; // end enum LocalScopeKind

    /**
     * @brief Represents result of lookup.
     */
    enum class LookupResult {
        NotFound,           //!< Not found in any scope.
        FoundInScope,       //!< Found a symbol in the scope.
        FoundOutOfScope,    //!< Found a symbol in parent's scope.
        FoundRedefinition,  //!< Found a symbol but the symbol has different signature.
    }; // end enum LookupResult

    /**
     * @brief .
     */
    enum class ReturnFlags {
        Invalid,
        Optional,
        MustReturn,
        NeverReturn
    }; // end enum ReturnFlags


    /**
     * @brief Represents local scope.
     */
    class LocalScope : private std::unordered_multimap<std::string_view, Symbol*> {
        using super = std::unordered_multimap<std::string_view, Symbol*>;

    private:
        LocalScope* m_parent;
        LocalScopeKind m_kind;
        DeclScope& m_decl;
        const int64_t m_depth;

        size_t m_count = 0;

    public:
        LocalScope(LocalScope* parent, LocalScopeKind kind, DeclScope& decl)
            : m_parent(parent), m_kind(kind), m_decl(decl), m_depth(parent == nullptr ? 0 : parent->m_depth + 1) {}

        int64_t GetScopeDepth() const { return m_depth; }

        /**
         * @brief Get a parent of the table.
         * @return a @c SymbolTable that represents the parent of the table.
         *         May return @c nullptr if the table does not have parent.
         */
        LocalScope* GetParent() const { return m_parent; }

        DeclScope& GetDeclScope() { return m_decl; }
        const DeclScope& GetDeclScope() const { return m_decl; }

        bool CanReturn() const;

        bool CanBreak() const {
            auto* self = this;

            while (self != nullptr) {
                if (self->m_kind == LocalScopeKind::Loop || self->m_kind == LocalScopeKind::Match) { return true; }
                if (self->m_kind == LocalScopeKind::Finally) { return false; }

                self = self->GetParent();
            }

            return false;
        }

        bool CanContinue() const {
            return CanBreak();
        }

        bool InFinally() const {
            auto* self = this;
            while (self != nullptr) {
                if (self->m_kind == LocalScopeKind::Finally) {
                    return true;
                }

                self = self->GetParent();
            }

            return false;
        }

        void CountNode() { ++m_count; }

        bool IsFirstNodeOfScope() const { return m_count == 0; }

        bool IsGlobal() const;

        bool IsRootOfDeclScope() const {
            // Current scope is DeclScope or Direct parent is DeclScope
            return (m_kind == LocalScopeKind::Root) || (GetParent()->m_kind == LocalScopeKind::Root);
        }

        /**
         * @brief
         */
        using Result = std::tuple<LookupResult, int64_t, Symbol*>;

        using super::begin;
        using super::end;

        /**
         * @brief Search a symbol with given name only in current table.
         * @param name a name of a symbol to find.
         * @return a @c Result that represents result of lookup.
         * @see Result
         */
        Result LookupLocal(std::string_view name) const;

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
        Result Lookup(std::string_view name) const;

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
    }; // end class LocalScope

    /**
     * @brief
     */
    enum class DeclScopeKind {
        Script,
        Function,
        Class,
        Method,
        Closure
    }; // end enum DeclScopeKind

    /**
     * @brief Represents a scope of the declaration.
     */
    class DeclScope {
    private:
        DeclScopeKind m_kind;
        LocalScope m_local;

    protected:
        DeclScope(DeclScopeKind kind, LocalScope* parent)
            : m_kind(kind), m_local(parent, LocalScopeKind::Root, *this) {}

    public:
        virtual bool CanImport() const { return false; }
        virtual bool CanExport() const { return false; }
        virtual bool InInstance() const { return false; }
        virtual bool CanReturn() const { return false; }
        virtual ReturnFlags GetReturnFlag() const { return ReturnFlags::Invalid; }

        LocalScope* GetRootScope() { return &m_local; }

        template <typename ScopeType>
        bool Is() const {
            static_assert(std::is_base_of_v<DeclScope, ScopeType>);

            return ScopeType::Kind == m_kind;
        }

        template <typename ScopeType>
        ScopeType* As() {
            static_assert(std::is_base_of_v<DeclScope, ScopeType>);

            return (ScopeType::Kind == m_kind) ? static_cast<ScopeType*>(this) : nullptr;
        }

        template <typename ScopeType>
        const ScopeType* As() const {
            static_assert(std::is_base_of_v<DeclScope, ScopeType>);

            return (ScopeType::Kind == m_kind) ? static_cast<const ScopeType*>(this) : nullptr;
        }
    }; // end class DeclScope

    /**
     * @brief
     */
    class ScriptScope : public DeclScope {
    public:
        static constexpr auto Kind = DeclScopeKind::Script;

    public:
        explicit ScriptScope(LocalScope* /*unused*/)
            : DeclScope(Kind, nullptr) {}

        bool CanImport() const override { return true; }

        bool CanExport() const override { return true; }
    }; // end class ScriptScope

    /**
     * @brief
     */
    class FunctionScope : public DeclScope {
    public:
        static constexpr auto Kind = DeclScopeKind::Function;

    public:
        explicit FunctionScope(LocalScope* parent)
            : DeclScope(Kind, parent) {}

        bool CanReturn() const override { return true; }

        ReturnFlags GetReturnFlag() const override { return ReturnFlags::Optional; }
    }; // end class FunctionScope

    /**
     * @brief
     */
    class ClassScope : public DeclScope {
    public:
        static constexpr auto Kind = DeclScopeKind::Class;

    public:
        explicit ClassScope(LocalScope* parent)
            : DeclScope(Kind, parent) {}
    }; // end class ClassScope

    /**
     * @brief
     */
    enum class MethodScopeKind {
        Initializer,
        Deinitializer,
        Method,
        StaticMethod,
        Operator,
        InplaceOperator,
        Getter,
        Setter,
        Action
    }; // end enum MethodScopeKind

    /**
     * @brief
     */
    class MethodScope : public DeclScope {
    public:
        static constexpr auto Kind = DeclScopeKind::Method;

    private:
        MethodScopeKind m_kind;
        bool m_hasInit = false;

        inline bool IsStatic() const {
            return (m_kind == MethodScopeKind::StaticMethod) || (m_kind == MethodScopeKind::Operator);
        }

    public:
        MethodScope(LocalScope* parent, MethodScopeKind kind)
            : DeclScope(Kind, parent), m_kind(kind) {}

        MethodScopeKind GetMethodKind() const { return m_kind; }

        bool InInstance() const override { return !IsStatic(); }

        bool CanReturn() const override { return true; }

        ReturnFlags GetReturnFlag() const override {
            switch (m_kind) {
                case MethodScopeKind::Initializer:
                case MethodScopeKind::Deinitializer:
                case MethodScopeKind::Setter:
                case MethodScopeKind::Action:
                    return ReturnFlags::NeverReturn;

                case MethodScopeKind::Getter:
                case MethodScopeKind::Operator:
                case MethodScopeKind::InplaceOperator:
                    return ReturnFlags::MustReturn;

                case MethodScopeKind::Method:
                case MethodScopeKind::StaticMethod:
                    return ReturnFlags::Optional;

                default:
                    NOT_REACHABLE;
            }
        }

        bool HasInitializerCall() const { return m_hasInit; }

        void SetInitializerCall() { m_hasInit = true; }
    }; // end class MethodScope

    /**
     * @brief
     */
    class ClosureScope : public DeclScope {
    public:
        static constexpr auto Kind = DeclScopeKind::Closure;

    private:
        std::set<Symbol*> m_boundedLocals;

    public:
        explicit ClosureScope(LocalScope* parent)
            : DeclScope(Kind, parent) {}

        bool CanReturn() const override { return true; }

        ReturnFlags GetReturnFlag() const override { return ReturnFlags::Optional; }

        bool IsBoundedLocal(Symbol* symbol) const { return m_boundedLocals.find(symbol) != m_boundedLocals.end(); }

        void AddBoundedLocal(Symbol* symbol) { m_boundedLocals.insert(symbol); }

        std::vector<Symbol*> GetBoundedLocals() const { return { m_boundedLocals.begin(), m_boundedLocals.end() }; }
    }; // end class ClosureScope

    inline bool LocalScope::CanReturn() const { return GetDeclScope().CanReturn() && !InFinally(); }

    inline bool LocalScope::IsGlobal() const { return GetDeclScope().Is<ScriptScope>() && GetParent() == nullptr; }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ANALYSIS_SCOPE_H
