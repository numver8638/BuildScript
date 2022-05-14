/*
 * Declarations.h
 * - Represents declaration in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_DECLARATIONS_H
#define BUILDSCRIPT_COMPILER_AST_DECLARATIONS_H

#include <string>
#include <string_view>
#include <vector>

#include <BuildScript/Assert.h>
#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/AST/OperatorKind.h>
#include <BuildScript/Compiler/AST/AccessFlags.h>
#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Utils/TrailObjects.h>

namespace BuildScript {
    class Context;
    class ParameterList; // Defined in <BuildScript/Compiler/AST/ParameterList.h>
    class Symbol; // Defined in <BuildScript/Compiler/Analysis/Symbol.h>

    enum class DeclarationKind {
        Invalid,

        Parameter,

        Script,

        Import,
        Export,
        Function,
        Class,
        Task,
        Variable,

        // Task members
        TaskInputs,
        TaskOutputs,
        TaskAction,
        TaskProperty,

        // Class members
        ClassInit,
        ClassDeinit,
        ClassField,
        ClassMethod,
        ClassProperty,
        ClassOperator
    }; // end enum DeclarationKind

    /**
     * @brief Represents invalid declaration.
     */
    class InvalidDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::Invalid;

    private:
        SourceRange m_range;

        explicit InvalidDeclaration(SourceRange range)
            : Declaration(Kind), m_range(range) {}

    public:
        /**
         * @brief Get a range of erroneous declaration.
         * @return the range of the declaration.
         */
        SourceRange GetRange() const { return m_range; }

        static InvalidDeclaration* Create(Context& context, SourceRange range);
    }; // end class InvalidDeclaration

    /**
     * @brief Represents declaration that has name.
     */
    class NamedDeclaration : public Declaration {
    private:
        Identifier m_name;
        Symbol* m_symbol = nullptr;

    protected:
        NamedDeclaration(DeclarationKind kind, Identifier name)
            : Declaration(kind), m_name(std::move(name)) {}

    public:
        /**
         * @brief Get a name of the declaration.
         * @return an @c Identifier that represents the name of the declaration.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief Get a symbol that represents the declaration.
         * @return a @c Symbol that represents the declaration.
         * @note This method must be called after call @c SetSymbol.
         */
        Symbol* GetSymbol() const {
            NEVER_BE_NULL(m_symbol);
            return m_symbol;
        }

        /**
         * @brief Set a symbol that represents the declaration.
         * @param symbol a @c Symbol that represents the declaration.
         * @warning This method must be called only once. Calling this more than one causes undefined behavior.
         */
        void SetSymbol(Symbol* symbol) {
            MUST_BE_NULL(m_symbol);
            m_symbol = symbol;
        }
    }; // end class NamedDeclaration

    /**
     * @brief
     */
    class Parameter final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::Parameter;

    private:
        explicit Parameter(Identifier name)
            : NamedDeclaration(Kind, std::move(name)) {}

    public:
        static Parameter* Create(Context& context, Identifier name);
    }; // end class Parameter

    /**
     * @brief Represents one script file. Root of AST is always this class.
     */
    class ScriptDeclaration final : public Declaration, TrailObjects<ScriptDeclaration, ASTNode*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = DeclarationKind::Script;

    private:
        std::string m_name;
        size_t m_count;

        ScriptDeclaration(std::string name, size_t count)
            : Declaration(Kind), m_name(std::move(name)), m_count(count) {}

        size_t GetTrailCount(OverloadToken<ASTNode*>) const { return m_count; } // TrailObjects support.

    public:
        /**
         * @brief Get name of the script. Same with relative path of the script.
         * @return the name of the script.
         */
        std::string_view GetName() const { return m_name; }

        /**
         * @brief Get children nodes of the declaration.
         * @return a @c TrailIterator that iterates children nodes of the declaration.
         */
        TrailIterator<ASTNode*> GetNodes() const { return GetTrailObjects<ASTNode*>(); }

        static ScriptDeclaration* Create(Context& context, std::string name, const std::vector<ASTNode*>& nodes);
    }; // end class ScriptDeclaration

    /**
     * @brief Represents import declaration.
     */
    class ImportDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::Import;

    private:
        SourcePosition m_import;
        Expression* m_path;

        ImportDeclaration(SourcePosition _import, Expression* path)
            : Declaration(Kind), m_import(_import), m_path(path) {}

    public:
        /**
         * @brief Get a position of 'import' keyword.
         * @return a @c SourcePosition representing where 'import' keyword positioned.
         */
        SourcePosition GetImportPosition() const { return m_import; }

        /**
         * @brief Get an expression represents the path of imported script.
         * @return an @c Expression of imported script path.
         */
        Expression* GetPath() const { return m_path; }

        static ImportDeclaration* Create(Context& context, SourcePosition _import, Expression* path);
    }; // end class ImportDeclaration

    /**
     * @brief Represents export declaration.
     */
    class ExportDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::Export;

    private:
        SourcePosition m_export;
        SourcePosition m_assign;
        Expression* m_value;

        ExportDeclaration(SourcePosition _export, Identifier name, SourcePosition assign,
                          Expression* value)
            : NamedDeclaration(Kind, std::move(name)), m_export(_export), m_assign(assign), m_value(value) {}

    public:
        /**
         * @brief Get a position of 'export' keyword.
         * @return a @c SourcePosition representing where 'export' keyword positioned.
         */
        SourcePosition GetExportPosition() const { return m_export; }

        /**
         * @brief Check the declaration has assigned value.
         * @return @c true if the declaration has assigned value otherwise @c false.
         */
        bool HasValue() const { return (bool)m_assign; }

        /**
         * @brief Get a position of '='.
         * @return a @c SourcePosition representing where '=' positioned.
         * @note Maybe empty if there's no assigned value.
         */
        SourcePosition GetAssignPosition() const { return m_assign; }

        /**
         * @brief Get an expression represents initial value of exported variable.
         * @return an @c Expression representing initial value of exported variable.
         * @note Maybe null if there's no assigned value.
         */
        Expression* GetValue() const { return m_value; }

        static ExportDeclaration*
        Create(Context& context, SourcePosition _export, Identifier name, SourcePosition assign, Expression* value);
    }; // end class ExportDeclaration

    /**
     * @brief Represents function declaration.
     */
    class FunctionDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::Function;

    private:
        SourcePosition m_def;
        ParameterList* m_params;
        Statement* m_body;

        FunctionDeclaration(SourcePosition def, Identifier name, ParameterList* param, Statement* body)
            : NamedDeclaration(Kind, std::move(name)), m_def(def), m_params(param), m_body(body) {}

    public:
        /**
         * @brief Get a position of 'def' keyword.
         * @return a @c SourcePosition representing where 'def' keyword positioned.
         */
        SourcePosition GetDefPosition() const { return m_def; }

        /**
         * @brief Get a parameter list of the function.
         * @return @c ParameterList representing parameter list of the function.
         */
        ParameterList* GetParameterList() const { return m_params; }

        /**
         * @brief Get body of the function.
         * @return a @c Statement representing body of the function.
         */
        Statement* GetBody() const { return m_body; }

        static FunctionDeclaration*
        Create(Context& context, SourcePosition def, Identifier name, ParameterList* param, Statement* body);
    }; // end class FunctionDeclaration

    /**
     * @brief Represents class declaration.
     */
    class ClassDeclaration final : public NamedDeclaration, TrailObjects<ClassDeclaration, Declaration*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = DeclarationKind::Class;

    private:
        SourcePosition m_class;
        SourcePosition m_extends;
        Identifier m_extendName;
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        ClassDeclaration(SourcePosition _class, Identifier name, SourcePosition extends,
                         Identifier extendName, SourcePosition open, SourcePosition close, size_t count)
            : NamedDeclaration(Kind, std::move(name)), m_class(_class), m_extends(extends),
              m_extendName(std::move(extendName)), m_open(open), m_close(close), m_count(count) {}

        size_t GetTrailCount(OverloadToken<Declaration*>) const { return m_count; } // TrailObjects support.

    public:
        /**
         * @brief Get a position of 'class' keyword.
         * @return a @c SourcePosition representing where 'class' keyword positioned.
         */
        SourcePosition GetClassPosition() const { return m_class; }

        /**
         * @brief Check the declaration inherits other class.
         * @return @c true if the declaration inherits other class otherwise @c false.
         */
        bool HasExtends() const { return (bool)m_extends; }

        /**
         * @brief Get a position of 'extends' keyword.
         * @return a @c SourcePosition representing where 'extends' keyword positioned.
         */
        SourcePosition GetExtendsPosition() const { return m_extends; }

        /**
         * @brief Get name of super class.
         * @return an @c Identifier representing name of super class.
         */
        const Identifier& GetExtendName() const { return m_extendName; }

        /**
         * @brief Get a position of '{'.
         * @return a @c SourcePosition representing where '{' positioned.
         */
        SourcePosition GetOpenBracePosition() const { return m_open; }

        /**
         * @brief Get a position of '}'.
         * @return a @c SourcePosition representing where '}' positioned.
         */
        SourcePosition GetCloseBracePosition() const { return m_close; }

        /**
         * @brief Get members of the declaration.
         * @return a @c TrailIterator that iterates members of the declaration.
         */
        TrailIterator<Declaration*> GetMembers() const { return GetTrailObjects<Declaration*>(); }

        static ClassDeclaration*
        Create(Context& context, SourcePosition _class, Identifier name, SourcePosition extends, Identifier extendName,
               SourcePosition open, const std::vector<Declaration*>& nodes, SourcePosition close);
    }; // end class ClassDeclaration

    /**
     * @brief Represents task declaration.
     */
    class TaskDeclaration final : public NamedDeclaration,
                                  TrailObjects<TaskDeclaration, Declaration*, Identifier, SourcePosition> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = DeclarationKind::Task;

    private:
        SourcePosition m_task;
        SourcePosition m_extends;
        Identifier m_extendName;
        SourcePosition m_dependsOn;
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;
        size_t m_depsCount;

        TaskDeclaration(SourcePosition task, Identifier name, SourcePosition extends,
                        Identifier extendName, SourcePosition dependsOn, SourcePosition open, SourcePosition close,
                        size_t count, size_t depsCount)
            : NamedDeclaration(Kind, std::move(name)), m_task(task), m_extends(extends),
              m_extendName(std::move(extendName)), m_dependsOn(dependsOn), m_open(open), m_close(close),
              m_count(count), m_depsCount(depsCount) {}

        size_t GetTrailCount(OverloadToken<Declaration*>) const { return m_count; } // TrailObjects support.
        size_t GetTrailCount(OverloadToken<Identifier>) const { return m_depsCount; }
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return (m_depsCount == 0) ? 0 : (m_depsCount - 1); }

    public:
        /**
         * @brief Get a position of 'task' keyword.
         * @return a @c SourcePosition representing where 'task' keyword positioned.
         */
        SourcePosition GetTaskPosition() const { return m_task; }

        /**
         * @brief Check the declaration extends other task.
         * @return @c true if the declaration extends other task otherwise @c false.
         */
        bool HasExtends() const { return (bool)m_extends; }

        /**
         * @brief Get a position of 'extends' keyword.
         * @return a @c SourcePosition representing where 'extends' keyword positioned.
         */
        SourcePosition GetExtendsPosition() const { return m_extends; }

        /**
         * @brief Get name of extended task.
         * @return an @c Identifier representing name of extended task.
         */
        const Identifier& GetExtendName() const { return m_extendName; }

        /**
         * @brief Check the declaration has dependencies.
         * @return @c true if the declaration has dependencies otherwise @c false.
         */
        bool HasDependsOn() const { return (bool)m_dependsOn; }

        /**
         * @brief Get a position of 'dependsOn' keyword.
         * @return a @c SourcePosition representing where 'dependsOn' keyword positioned.
         */
        SourcePosition GetDependsOnPosition() const { return m_dependsOn; }

        /**
         * @brief Get dependencies.
         * @return a @c TrailIterator that iterates @c Identifier representing dependency.
         */
        TrailIterator<Identifier> GetDependencyNames() const { return GetTrailObjects<Identifier>(); }

        /**
         * @brief Get positions of commas.
         * @return a @c TrailIterator that iterates @c SourcePosition representing where comma positioned.
         */
        TrailIterator<SourcePosition> GetCommaPositions() const { return GetTrailObjects<SourcePosition>(); }

        /**
         * @brief Get a position of '{'.
         * @return a @c SourcePosition representing where '{' positioned.
         */
        SourcePosition GetOpenBracePosition() const { return m_open; }

        /**
         * @brief Get a position of '}'.
         * @return a @c SourcePosition representing where '}' positioned.
         */
        SourcePosition GetCloseBracePosition() const { return m_close; }

        /**
         * @brief Get members of the declaration.
         * @return a @c TrailIterator that iterates members of the declaration.
         */
        TrailIterator<Declaration*> GetMembers() const { return GetTrailObjects<Declaration*>(); }

        static TaskDeclaration*
        Create(Context& context, SourcePosition task, Identifier name, SourcePosition extends, Identifier extendName,
               SourcePosition dependsOn, const std::vector<Identifier>& depnames,
               const std::vector<SourcePosition>& commas, SourcePosition open, const std::vector<Declaration*>& nodes,
               SourcePosition close);
    }; // end class TaskDeclaration

    /**
     * @brief Represents variable declaration.
     */
    class VariableDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::Variable;

    private:
        SourcePosition m_keyword;
        AccessFlags m_flags;
        SourcePosition m_assign;
        Expression* m_value;

        VariableDeclaration(SourcePosition key, AccessFlags flags, Identifier name, SourcePosition assign,
                            Expression* value)
            : NamedDeclaration(Kind, std::move(name)), m_keyword(key), m_flags(flags), m_assign(assign),
              m_value(value) {
            assert((m_flags != AccessFlags::Static) && "kind of variable cannot be 'static'");
        }

    public:
        /**
         * @brief Get a position of 'var' keyword.
         * @return a @c SourcePosition representing where 'var' keyword positioned.
         * @note Maybe empty if variable is const.
         */
        SourcePosition GetVarPosition() const {
            return (m_flags == AccessFlags::ReadWrite) ? m_keyword : SourcePosition::Empty();
        }

        /**
         * @brief Get a position of 'const' keyword.
         * @return a @c SourcePosition representing where 'const' keyword positioned.
         * @note Maybe empty if variable is not const.
         */
        SourcePosition GetConstPosition() const {
            return (m_flags == AccessFlags::Const) ? m_keyword : SourcePosition::Empty();
        }

        /**
         * @brief Check that the declaration is readonly.
         * @return @c true if the declaration is readonly otherwise @c false.
         */
        bool IsConst() const { return (m_flags == AccessFlags::Const); }

        /**
         * @brief Get an @c AccessFlags that represents
         * @return
         * @see BuildScript::AccessFlags
         */
        AccessFlags GetAccessFlag() const { return m_flags; }

        /**
         * @brief Get a position of '='.
         * @return a @c SourcePosition representing where '=' positioned.
         */
        SourcePosition GetAssignPosition() const { return m_assign; }

        /**
         * @brief Get an expression represents initial value of the declaration.
         * @return an @c Expression representing initial value.
         */
        Expression* GetValue() const { return m_value; }

        static VariableDeclaration*
        Create(Context& context, SourcePosition pos, AccessFlags flags, Identifier name, SourcePosition assign,
               Expression* value);
    }; // end class VariableDeclaration

    //
    // Task members
    //
    /**
     * @brief Represents inputs in task declaration.
     */
    class TaskInputsDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::TaskInputs;

    private:
        SourcePosition m_inputs;
        Expression* m_inputsValue;
        SourcePosition m_with;
        Expression* m_withValue;

        TaskInputsDeclaration(SourcePosition inputs, Expression* inputsValue, SourcePosition with,
                              Expression* withValue)
            : Declaration(Kind), m_inputs(inputs), m_inputsValue(inputsValue), m_with(with),
              m_withValue(withValue) {}

    public:
        /**
         * @brief Get a position of 'inputs' keyword.
         * @return a @c SourcePosition representing where 'inputs' keyword positioned.
         */
        SourcePosition GetInputsPosition() const { return m_inputs; }

        /**
         * @brief
         * @return
         */
        Expression* GetInputsValue() const { return m_inputsValue; }

        /**
         * @brief
         * @return
         */
        bool HasWith() const { return (bool)m_with; }

        /**
         * @brief Get a position of 'with' keyword.
         * @return a @c SourcePosition representing where 'with' keyword positioned.
         */
        SourcePosition GetWithPosition() const { return m_with; }

        /**
         * @brief
         * @return
         */
        Expression* GetWithValue() const { return m_withValue; }

        static TaskInputsDeclaration*
        Create(Context& context, SourcePosition inputs, Expression* inputsValue, SourcePosition with,
               Expression* withValue);
    }; // end class TaskInputsDeclaration

    /**
     * @brief Represents outputs in task declaration.
     */
    class TaskOutputsDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::TaskOutputs;

    private:
        SourcePosition m_outputs;
        Expression* m_outputsValue;
        SourcePosition m_from;
        Expression* m_fromValue;

        TaskOutputsDeclaration(SourcePosition outputs, Expression* outputsValue, SourcePosition from,
                               Expression* fromValue)
            : Declaration(Kind), m_outputs(outputs), m_outputsValue(outputsValue), m_from(from),
              m_fromValue(fromValue) {}

    public:
        /**
         * @brief Get a position of 'outputs' keyword.
         * @return a @c SourcePosition representing where 'outputs' keyword positioned.
         */
        SourcePosition GetOutputsPosition() const { return m_outputs; }

        /**
         * @brief
         * @return
         */
        Expression* GetOutputsValue() const { return m_outputsValue; }

        /**
         * @brief
         * @return
         */
        bool HasFrom() const { return (bool)m_from; }

        /**
         * @brief Get a position of 'from' keyword.
         * @return a @c SourcePosition representing where 'from' keyword positioned.
         */
        SourcePosition GetFromPosition() const { return m_from; }

        /**
         * @brief
         * @return
         */
        Expression* GetFromValue() const { return m_fromValue; }

        static TaskOutputsDeclaration*
        Create(Context& context, SourcePosition outputs, Expression* outputsValue, SourcePosition from,
               Expression* fromValue);
    }; // end class TaskOutputsDeclaration

    enum class ActionKind {
        DoFirst,    //!< Represents 'doFirst' action.
        Do,         //!< Represents 'do' action.
        DoLast      //!< Represents 'doLast' action.
    }; // end enum ActionKind

    /**
     * @brief Represents action('doFirst', 'do' and 'doLast') in task declaration.
     */
    class TaskActionDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::TaskAction;

    private:
        ActionKind m_kind;
        SourcePosition m_pos;
        Statement* m_body;
        Symbol* m_symbol = nullptr;

        TaskActionDeclaration(ActionKind kind, SourcePosition pos, Statement* body)
            : Declaration(Kind), m_kind(kind), m_pos(pos), m_body(body) {}

    public:
        /**
         * @brief Get kind of action declaration.
         * @return an @c ActionKind representing the declaration represents.
         */
        ActionKind GetActionKind() const { return m_kind; }

        /**
         * @brief Get a position of the keyword.
         * @return a @c SourcePosition representing where the keyword positioned.
         */
        SourcePosition GetKeywordPosition() const { return m_pos; }

        /**
         * @brief Get body of the action.
         * @return a @c Statement representing body of the action.
         */
        Statement* GetBody() const { return m_body; }

        /**
         * @brief
         * @return
         */
        Symbol* GetSymbol() const {
            NEVER_BE_NULL(m_symbol);
            return m_symbol;
        }

        /**
         * @brief
         * @param symbol
         */
        void SetSymbol(Symbol* symbol) {
            MUST_BE_NULL(m_symbol);
            m_symbol = symbol;
        }

        static TaskActionDeclaration* Create(Context& context, ActionKind kind, SourcePosition pos, Statement* body);
    }; // end class TaskActionDeclaration

    /**
     * @brief Represents property in task declaration.
     */
    class TaskPropertyDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::TaskProperty;

    private:
        SourcePosition m_assign;
        Expression* m_value;

        TaskPropertyDeclaration(Identifier name, SourcePosition assign, Expression* value)
            : NamedDeclaration(Kind, std::move(name)), m_assign(assign), m_value(value) {}

    public:
        /**
         * @brief Get a position of '='.
         * @return a @c SourcePosition representing where '=' positioned.
         */
        SourcePosition GetAssignPosition() const { return m_assign; }

        /**
         * @brief Get an expression represents initial value of the property.
         * @return an @c Expression representing initial value.
         */
        Expression* GetValue() const { return m_value; }

        static TaskPropertyDeclaration*
        Create(Context& context, Identifier name, SourcePosition assign, Expression* value);
    }; // end class TaskPropertyDeclaration

    //
    // Class members
    //
    /**
     * @brief Represents initializer in class declaration.
     */
    class ClassInitDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassInit;

    private:
        SourcePosition m_init;
        ParameterList* m_params;
        Statement* m_body;
        bool m_initCall = false;

        ClassInitDeclaration(SourcePosition init, ParameterList* params, Statement* body)
            : NamedDeclaration(Kind, Identifier{ SourceRange{ init }, "<init>" }),
              m_init(init), m_params(params), m_body(body) {}

    public:
        /**
         * @brief Get a position of 'init' keyword.
         * @return a @c SourcePosition representing where 'init' keyword positioned.
         */
        SourcePosition GetInitPosition() const { return m_init; }

        /**
         * @brief Get parameters of the initializer.
         * @return @c ParameterList representing parameters of the initializer.
         */
        ParameterList* GetParameterList() const { return m_params; }

        /**
         * @brief Get body of the initializer.
         * @return a @c Statement representing body of the initializer.
         */
        Statement* GetBody() const { return m_body; }

        bool HasInitializerCall() const { return m_initCall; }

        void SetInitializerCall() { m_initCall = true; }

        static ClassInitDeclaration*
        Create(Context& context, SourcePosition init, ParameterList* params, Statement* body);
    }; // end class ClassInitDeclaration

    /**
     * @brief Represent deinitializer in class declaration.
     */
    class ClassDeinitDeclaration final : public NamedDeclaration  {
    public:
        static constexpr auto Kind = DeclarationKind::ClassDeinit;

    private:
        SourcePosition m_deinit;
        Statement* m_body;

        ClassDeinitDeclaration(SourcePosition deinit, Statement* body)
            : NamedDeclaration(Kind, Identifier{ SourceRange{ deinit }, "<deinit>" }), m_deinit(deinit), m_body(body) {}

    public:
        /**
         * @brief Get a position of 'deinit' keyword.
         * @return a @c SourcePosition representing where 'deinit' keyword positioned.
         */
        SourcePosition GetDeinitPosition() const { return m_deinit; }

        /**
         * @brief Get body of the deinitializer.
         * @return a @c Statement representing body of the deinitializer.
         */
        Statement* GetBody() const { return m_body; }

        static ClassDeinitDeclaration* Create(Context& context, SourcePosition deinit, Statement* body);
    }; // end class ClassDeinitDeclaration

    /**
     * @brief Represents field in class declaration.
     */
    class ClassFieldDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassField;

    private:
        SourcePosition m_keyword;
        AccessFlags m_flags;
        SourcePosition m_assign;
        Expression* m_value;

        ClassFieldDeclaration(SourcePosition keyword, AccessFlags flags, Identifier name,
                              SourcePosition assign, Expression* value)
            : NamedDeclaration(Kind, std::move(name)), m_keyword(keyword), m_flags(flags), m_assign(assign),
              m_value(value) {
            assert((m_flags != AccessFlags::ReadWrite) && "kind of field cannot be 'var'.");
        }

    public:
        /**
         * @brief
         * @return
         */
        bool IsConst() const { return (m_flags == AccessFlags::Const); }

        /**
         * @brief
         * @return
         */
        bool IsStatic() const { return (m_flags == AccessFlags::Static); }

        /**
         * @brief
         * @return
         */
        AccessFlags GetAccessFlag() const { return m_flags; }

        /**
         * @brief Get a position of 'static' keyword.
         * @return a @c SourcePosition representing where 'static' keyword positioned.
         * @note Return value may be empty if field is not static.
         */
        SourcePosition GetStaticPosition() const { return IsStatic() ? m_keyword : SourcePosition::Empty(); }

        /**
         * @brief Get a position of 'const' keyword.
         * @return a @c SourcePosition representing where 'const' keyword positioned.
         * @note Return value may be empty if field is not const.
         */
        SourcePosition GetConstPosition() const { return IsConst() ? m_keyword : SourcePosition::Empty(); }

        /**
         * @brief Get a position of '='.
         * @return a @c SourcePosition representing where '=' positioned.
         */
        SourcePosition GetAssignPosition() const { return m_assign; }

        /**
         * @brief Get an expression represents initial value of the declaration.
         * @return an @c Expression representing initial value.
         */
        Expression* GetValue() const { return m_value; }

        static ClassFieldDeclaration*
        Create(Context& context, SourcePosition pos, AccessFlags flags, Identifier name, SourcePosition assign,
               Expression* value);
    }; // end class ClassFieldDeclaration

    /**
     * @brief Represents method in class declaration.
     */
    class ClassMethodDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassMethod;

    private:
        SourcePosition m_static;
        SourcePosition m_def;
        OperatorKind m_op;
        SourcePosition m_pos;
        ParameterList* m_params;
        Statement* m_body;

        ClassMethodDeclaration(SourcePosition _static, SourcePosition def, Identifier name, OperatorKind op,
                               SourcePosition pos, ParameterList* params, Statement* body)
            : NamedDeclaration(Kind, std::move(name)), m_static(_static), m_def(def), m_op(op), m_pos(pos),
              m_params(params), m_body(body) {}

    public:
        /**
         * @brief Check the method is static method.
         * @return @c true if the method is static method otherwise @c false.
         */
        bool IsStatic() const { return (bool)m_static; }

        /**
         * @brief Get a position of 'static' keyword.
         * @return a @c SourcePosition representing where 'static' keyword positioned.
         * @note Return value may be empty if method is not static.
         */
        SourcePosition GetStaticPosition() const { return m_static; }

        /**
         * @brief Get a position of 'def' keyword.
         * @return a @c SourcePosition representing where 'def' keyword positioned.
         */
        SourcePosition GetDefPosition() const { return m_def; }

        /**
         * @brief Check the method overloads operator.
         * @return @c true if the method overloads operator otherwise @c false.
         */
        bool IsOperator() const { return m_op != OperatorKind::Invalid; }

        /**
         * @brief Get an @c OperatorKind that the method overloads.
         * @return an @c OperatorKind that the method overloads.
         * @note May return @c OperatorKind::Invalid if the method does not overload operator.
         */
        OperatorKind GetOperator() const { return m_op; }

        /**
         * @brief Get a position of the operator.
         * @return a @c SourcePosition representing where operator positioned.
         * @note Return value may be empty if the method does not overload operator.
         */
        SourcePosition GetOperatorPosition() const { return m_pos; }

        /**
         * @brief Get parameters of the method.
         * @return @c ParameterList representing parameters of the method.
         */
        ParameterList* GetParameterList() const { return m_params; }

        /**
         * @brief Get body of the method.
         * @return a @c Statement representing body of the method.
         */
        Statement* GetBody() const { return m_body; }

        static ClassMethodDeclaration*
        CreateMethod(Context& context, SourcePosition _static, SourcePosition def, Identifier name, ParameterList* params,
                     Statement* body);

        static ClassMethodDeclaration*
        CreateOperator(Context& context, SourcePosition def, OperatorKind op, SourcePosition pos, ParameterList* params,
                       Statement* body);
    }; // end class ClassMethodDeclaration

    /**
     * @brief Represents property(getter or setter) in class declaration.
     */
    class ClassPropertyDeclaration final : public NamedDeclaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassProperty;

    private:
        SourcePosition m_keyword;
        SourcePosition m_subscript;
        bool m_isGetter;
        Statement* m_body;
        Symbol* m_method = nullptr;

        ClassPropertyDeclaration(SourcePosition keyword, Identifier name, SourcePosition subscript, bool isGetter,
                                 Statement* body)
            : NamedDeclaration(Kind, std::move(name)), m_keyword(keyword), m_subscript(subscript),
              m_isGetter(isGetter), m_body(body) {}

    public:
        /**
         * @brief Check that the property is getter.
         * @return @c true if the property is getter otherwise @c false.
         */
        bool IsGetter() const { return m_isGetter; }

        /**
         * @brief Check that the property is setter.
         * @return @c true if the property is setter otherwise @c false.
         */
        bool IsSetter() const { return !m_isGetter; }

        /**
         * @brief Check that the property is subscript overloading.
         * @return @c true if the property is subscript overloading otherwise @c false.
         */
        bool IsSubscript() const { return (bool)m_subscript; }

        /**
         * @brief Get a position of 'get' keyword.
         * @return a @c SourcePosition representing where 'get' keyword positioned.
         * @note Return value may be empty if property is not getter.
         */
        SourcePosition GetGetPosition() const { return m_isGetter ? m_keyword : SourcePosition::Empty(); }

        /**
         * @brief Get a position of 'set' keyword.
         * @return a @c SourcePosition representing where 'set' keyword positioned.
         * @note Return value may be empty if property is not setter.
         */
        SourcePosition GetSetPosition() const { return m_isGetter ? SourcePosition::Empty() : m_keyword; }

        /**
         * @brief Get a position of 'subscript' keyword.
         * @return a @c SourcePosition representing where 'subscript' keyword positioned.
         * @note Return value may be empty if property is not subscript.
         */
        SourcePosition GetSubscriptPosition() const { return m_subscript; }

        /**
         * @brief Get body of the property.
         * @return a @c Statement representing body of the property.
         */
        Statement* GetBody() const { return m_body; }

        /**
         * @brief
         * @return
         */
        Symbol* GetMethodSymbol() const {
            NEVER_BE_NULL(m_method);
            return m_method;
        }

        /**
         * @brief
         * @param symbol
         */
        void SetMethodSymbol(Symbol* symbol) {
            MUST_BE_NULL(m_method);
            m_method = symbol;
        }

        static ClassPropertyDeclaration*
        Create(Context& context, SourcePosition get, SourcePosition set, Identifier name, Statement* body);

        static ClassPropertyDeclaration*
        CreateSubscript(Context& context, SourcePosition get, SourcePosition set, SourcePosition subscript,
                        Statement* body);
    }; // end class ClassPropertyDeclaration
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_DECLARATIONS_H