/*
 * Symbol.h
 * -  Represents declared identifier in the source.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_SYMBOL_SYMBOL_H
#define BUILDSCRIPT_COMPILER_SYMBOL_SYMBOL_H

#include <type_traits>
#include <string>
#include <string_view>

#include <BuildScript/Assert.h>
#include <BuildScript/Compiler/AST/AccessFlags.h>
#include <BuildScript/Compiler/ManagedObject.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Utils/Trilean.h>

namespace BuildScript {
    /**
     * @brief Represents a type of the symbol.
     */
    enum class SymbolType {
        Undeclared,     //!< Used but undeclared symbol.
        Variable,       //!< Declared as variable, parameter or etc.
        Type,           //!< Declared as type.
        Function,       //!< Declared as function.
        Class,          //!< Declared as class.
        Task,           //!< Declared as task.
        Field,          //!< Declared as class field.
        Method,         //!< Declared as class method.
        Property,       //!< Declared as class property.
        Closure         //!< Represents a closure. It does not exposed directly.
    }; // end enum SymbolType

    /**
     * @brief Represents declared identifier in the source.
     */
    class Symbol : public ManagedObject {
    public:
        static constexpr std::string_view ArgumentSeparator = "$";
        static constexpr std::string_view MemberSeparator = ".";
        static constexpr std::string_view VariadicSuffix = "+";
        static constexpr std::string_view ClassInitializerName = "<cinit>";
        static constexpr std::string_view InitializerName = "<init>";
        static constexpr std::string_view DeinitializerName = "<deinit>";
        static constexpr std::string_view DoClauseName = "action";
        static constexpr std::string_view DoFirstClauseName = "actionBefore";
        static constexpr std::string_view DoLastClauseName = "actionAfter";

    private:
        SymbolType m_type;
        std::string m_name;
        SourcePosition m_pos;

    protected:
        Symbol(SymbolType type, std::string_view name, SourcePosition pos)
            : m_type(type), m_name(name), m_pos(pos) {}

        Symbol(SymbolType type, const Identifier& name)
            : m_type(type), m_name(name.GetString()), m_pos(name.GetPosition()) {}


    public:
        /**
         * @brief Get @c SymbolType that represents the type of the symbol.
         * @return a @c SymbolType that represents the type of the symbol.
         */
        SymbolType GetType() const { return m_type; }

        /**
         * @brief Get simple name of the symbol.
         * @return a @c std::string_view that represents the name of the symbol.
         */
        std::string_view GetName() const { return m_name; }

        /**
         * @brief Get mangled name of the symbol.
         * @return a @c std::string that represents mangled name of the symbol.
         */
        virtual std::string GetMangledName() const { return m_name; }

        /**
         * @brief Get a name of the symbol with detail.
         * @return a @c std::string that represents the name of the symbol with detail.
         */
        virtual std::string GetDescriptiveName() const = 0;

        /**
         * @brief Get a position of the symbol.
         * @return a @c SourcePosition that represent where the symbol declared.
         */
        SourcePosition GetPosition() const { return m_pos; }

        /**
         * @brief Check that the symbol is writable.
         * @return a @c Trilean that represents the symbol is writable.
         * @see BuildScript::Trilean
         */
        virtual Trilean IsWritable() const = 0;

        /**
         * @brief Check that the symbol is initialized.
         * @return a @c Trilean that represents the symbol is initialized.
         * @see BuildScript::Trilean
         */
        virtual Trilean IsInitialized() const = 0;

        /**
         * @brief Cast the symbol to desired symbol.
         * @tparam SymbolType the type derived from @c Symbol.
         * @return casted pointer to @c SymbolType if the symbol is @c SymbolType otherwise @c nullptr.
         */
        template <typename SymbolType>
        SymbolType* As() {
            static_assert(std::is_base_of_v<Symbol, SymbolType>, "SymbolType is not base of Symbol.");
            return (SymbolType::Type == GetType()) ? static_cast<SymbolType*>(this) : nullptr;
        }

        /**
         * @copydoc BuildScript::Symbol::As()
         */
        template <typename SymbolType>
        const SymbolType* As() const {
            static_assert(std::is_base_of_v<Symbol, SymbolType>, "SymbolType is not base of Symbol.");
            return (SymbolType::Type == GetType()) ? static_cast<const SymbolType*>(this) : nullptr;
        }
    }; // end class Symbol

    /**
     * @brief Represents accessed but undeclared symbol.
     */
    class UndeclaredSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Undeclared;

    public:
        UndeclaredSymbol(std::string_view name, SourcePosition pos)
            : Symbol(Type, name, pos) {}

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return Trilean::Unknown; }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::Unknown; }
    }; // end class UndeclaredSymbol

    /**
     * @brief Represents a type of the variable.
     */
    enum class VariableType {
        Global,     //!< Represents global variable.
        Local,      //!< Represents local variable.
        Exported,   //!< Represents exported variable.
        Parameter,  //!< Represents a parameter.
        Except,     //!< Represents captured exception variable.
        Implicit    //!< Represents implicit parameter.
    }; // end enum VariableType

    /**
     * @brief Represent every variable-like symbol.
     */
    class VariableSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Variable;

        // Implicit variable names
        static constexpr std::string_view Inputs = "inputs";
        static constexpr std::string_view Input = "input";
        static constexpr std::string_view Outputs = "outputs";
        static constexpr std::string_view Output = "output";
        static constexpr std::string_view Index = "index";
        static constexpr std::string_view Value = "value";

    private:
        VariableType m_type;
        bool m_readonly;
        bool m_initialized = false;

    public:
        VariableSymbol(const Identifier& name, VariableType type, bool readonly)
            : Symbol(Type, name), m_type(type), m_readonly(readonly) {
            if (type != VariableType::Global && type != VariableType::Local) {
                m_initialized = true;
            }
        }

        VariableSymbol(std::string_view name, SourcePosition pos, VariableType type, bool readonly)
            : Symbol(Type, name, pos), m_type(type), m_readonly(readonly) {
            if (type != VariableType::Global && type != VariableType::Local) {
                m_initialized = true;
            }
        }

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return ToTrilean(!m_readonly); }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return ToTrilean(m_initialized); }

        /**
         * @brief Get a type of the variable.
         * @return a @c VariableType that represents a type of the variable.
         */
        VariableType GetVariableType() const { return m_type; }

        /**
         * @brief Mark the variable initialized.
         */
        void SetInitialized() { m_initialized = true; }

        /**
         * @brief Set declared variable to exported variable.
         */
        void SetExported() {
            assert(!(m_type == VariableType::Parameter || m_type == VariableType::Except || m_type == VariableType::Implicit)
                   && "cannot export parameter, except and implicit variable.");

            m_type = VariableType::Exported;
        }
    }; // end class VariableSymbol

    /**
     * @brief Represents a symbol of the function.
     */
    class FunctionSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Function;

    private:
        int m_argc;
        bool m_vararg;

    public:
        FunctionSymbol(const Identifier& name, int argc, bool vararg)
            : Symbol(Type, name), m_argc(argc), m_vararg(vararg) {}

        /**
         * @copydoc BuildScript::Symbol::GetMangledName()
         */
        std::string GetMangledName() const override;

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return Trilean::False; }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::True; }

        /**
         * @brief Get a count of arguments of the function.
         * @return a count of argument of the function.
         */
        int GetArgumentCount() const { return m_argc; }

        /**
         * @brief Check that the function has variadic argument.
         * @return @c true if the function has variadic argument otherwise @c false.
         */
        bool HasVariadicArgument() const { return m_vararg; }
    }; // end class FunctionSymbol

    /**
     * @brief Represents a symbol that is type name.
     */
    class TypeSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Type;

        explicit TypeSymbol(std::string_view name)
            : Symbol(Type, name, SourcePosition()) {}

        explicit TypeSymbol(const Identifier& name)
            : Symbol(Type, name) {}

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return Trilean::False; }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::True; }
    }; // end class TypeSymbol

    /**
     * @brief Represents a symbol of the class.
     */
    class ClassSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Class;

        explicit ClassSymbol(const Identifier& name)
            : Symbol(Type, name) {}

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return Trilean::False; }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::True; }
    }; // end class ClassSymbol

    /**
     * @brief Represents a symbol of the task.
     */
    class TaskSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Task;

        explicit TaskSymbol(const Identifier& name)
            : Symbol(Type, name) {}

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return Trilean::False; }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::True; }
    }; // end class TaskSymbol

    /**
     * @brief Represents a symbol of the field.
     */
    class FieldSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Field;

    private:
        AccessFlags m_flags;
        Symbol* m_owner;
        bool m_initialized = false;

    public:
        FieldSymbol(const Identifier& name, AccessFlags flags, Symbol* owner)
            : Symbol(Type, name), m_flags(flags), m_owner(owner) {}

        /**
         * @copydoc BuildScript::Symbol::GetMangledName()
         */
        std::string GetMangledName() const override;

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return ToTrilean(m_flags != AccessFlags::Const); }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return ToTrilean(m_initialized); }

        /**
         * @brief Get a symbol which owns the field.
         * @return a @c Symbol that represents symbol which owns the field.
         */
        Symbol* GetOwner() const { return m_owner; }

        /**
         * @brief Mark the field initialized.
         */
        void SetInitialized() { m_initialized = true; }
    }; // end class FieldSymbol

    /**
     * @brief Represents a symbol of the method.
     */
    class MethodSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Method;

    private:
        int m_argc;
        bool m_vararg;
        bool m_static;
        Symbol* m_owner;

    public:
        MethodSymbol(const Identifier& name, int argc, bool vararg, bool isStatic, Symbol* owner)
            : Symbol(Type, name), m_argc(argc), m_vararg(vararg), m_static(isStatic), m_owner(owner) {}

        MethodSymbol(std::string_view name, SourcePosition pos, int argc, bool vararg, bool isStatic, Symbol* owner)
            : Symbol(Type, name, pos), m_argc(argc), m_vararg(vararg), m_static(isStatic), m_owner(owner) {}

        /**
         * @copydoc BuildScript::Symbol::GetMangledName()
         */
        std::string GetMangledName() const override;

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return Trilean::False; }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::True; }

        /**
         * @brief Get a symbol which owns the method.
         * @return a @c Symbol that represents symbol which owns the method.
         */
        Symbol* GetOwner() const { return m_owner; }

        /**
         * @brief Get a count of arguments of the method.
         * @return a count of argument of the method.
         */
        int GetArgumentCount() const { return m_argc; }

        /**
         * @brief Check that the method has variadic argument.
         * @return @c true if the method has variadic argument otherwise @c false.
         */
        bool HasVariadicArgument() const { return m_vararg; }

        /**
         * @brief Check that the method is static.
         * @return @c true if the method is static method otherwise @c false.
         */
        bool IsStatic() const { return m_static; }
    }; // end class MethodSymbol

    /**
     * @brief Represents a symbol of the property.
     */
    class PropertySymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Property;

    private:
        Symbol* m_owner;
        MethodSymbol* m_getter = nullptr;
        MethodSymbol* m_setter = nullptr;

    public:
        PropertySymbol(const Identifier& name, Symbol* owner)
            : Symbol(Type, name), m_owner(owner) {}

        /**
         * @copydoc BuildScript::Symbol::GetMangledName()
         */
        std::string GetMangledName() const override;

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return ToTrilean(m_setter != nullptr); }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::True; }

        /**
         * @brief Get a symbol which owns the property.
         * @return a @c Symbol that represents symbol which owns the property.
         */
        Symbol* GetOwner() const { return m_owner; }

        /**
         * @brief Check that the property has getter.
         * @return @c true if the property has getter otherwise @c false.
         */
        bool HasGetter() const { return m_getter != nullptr; }

        /**
         * @brief Check that the property has setter.
         * @return @c true if the property has setter otherwise @c false.
         */
        bool HasSetter() const { return m_setter != nullptr; }

        /**
         * @brief Set a symbol of getter.
         * @param symbol a @c MethodSymbol that represents implementation of getter.
         * @note This method must be called once. Calling this method more than once may cause undefined behavior.
         */
        void SetGetter(MethodSymbol* symbol) {
            MUST_BE_NULL(m_getter);
            m_getter = symbol;
        }

        /**
         * @brief Set a symbol of setter.
         * @param symbol a @c MethodSymbol that represents implementation of setter.
         * @note This method must be called once. Calling this method more than once may cause undefined behavior.
         */
        void SetSetter(MethodSymbol* symbol) {
            MUST_BE_NULL(m_setter);
            m_setter = symbol;
        }
    }; // end class PropertySymbol

    /**
     * @brief Represents a symbol of the closure.
     */
    class ClosureSymbol final : public Symbol {
    public:
        static constexpr auto Type = SymbolType::Closure;

    private:
        static unsigned m_counter;

    public:
        explicit ClosureSymbol(SourcePosition pos);

        /**
         * @copydoc BuildScript::Symbol::GetDescriptiveName()
         */
        std::string GetDescriptiveName() const override;

        /**
         * @copydoc BuildScript::Symbol::IsWritable()
         */
        Trilean IsWritable() const override { return Trilean::False; }

        /**
         * @copydoc BuildScript::Symbol::IsInitialized()
         */
        Trilean IsInitialized() const override { return Trilean::True; }
    }; // end class ClosureSymbol
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_SYMBOL_SYMBOL_H