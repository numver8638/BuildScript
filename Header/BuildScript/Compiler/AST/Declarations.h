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

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/AST/OperatorKind.h>
#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Utils/TrailObjects.h>

namespace BuildScript {
    class Parameters; // Defined in <BuildScript/Compiler/AST/Parameters.h>

    enum class DeclarationKind {
        Invalid,

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
        explicit InvalidDeclaration(SourceRange range)
            : Declaration(Kind, range) {}

    public:
        static InvalidDeclaration* Create(Context& context, SourceRange range);
    }; // end class InvalidDeclaration

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

        ScriptDeclaration(SourceRange range, std::string name, size_t count)
            : Declaration(Kind, range), m_name(std::move(name)), m_count(count) {}

        size_t GetTrailCount(OverloadToken<ASTNode*>) const { return m_count; } // TrailObjects support.

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get name of the script. Same with relative path of the script.
         * @return the name of the script.
         */
        std::string_view GetName() const { return m_name; }

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

        ImportDeclaration(SourceRange range, SourcePosition _import, Expression* path)
            : Declaration(Kind, range), m_import(_import), m_path(path) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

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
        const Expression* GetPath() const { return m_path; }

        static ImportDeclaration* Create(Context& context, SourcePosition _import, Expression* path);
    }; // end class ImportDeclaration

    /**
     * @brief Represents export declaration.
     */
    class ExportDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::Export;

    private:
        SourcePosition m_export;
        Identifier m_name;
        SourcePosition m_assign;
        Expression* m_value;

        ExportDeclaration(SourceRange range, SourcePosition _export, Identifier name, SourcePosition assign,
                          Expression* value)
            : Declaration(Kind, range), m_export(_export), m_name(std::move(name)), m_assign(assign), m_value(value) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'export' keyword.
         * @return a @c SourcePosition representing where 'export' keyword positioned.
         */
        SourcePosition GetExportPosition() const { return m_export; }

        /**
         * @brief Get name of exported variable.
         * @return an @c Identifier representing name of exported variable.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief
         * @return
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
        const Expression* GetValue() const { return m_value; }

        static ExportDeclaration*
        Create(Context& context, SourcePosition _export, Identifier name, SourcePosition assign, Expression* value);
    }; // end class ExportDeclaration

    /**
     * @brief Represents function declaration.
     */
    class FunctionDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::Function;

    private:
        SourcePosition m_def;
        Identifier m_name;
        Parameters* m_params;
        Statement* m_body;

        FunctionDeclaration(SourceRange range, SourcePosition def, Identifier name, Parameters* param, Statement* body)
            : Declaration(Kind, range), m_def(def), m_name(std::move(name)), m_params(param), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'def' keyword.
         * @return a @c SourcePosition representing where 'def' keyword positioned.
         */
        SourcePosition GetDefPosition() const { return m_def; }

        /**
         * @brief Get name of the function.
         * @return an @c Identifier representing name of the function.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief Get parameters of the function.
         * @return @c Parameters representing parameters of the function.
         */
        const Parameters* GetParameters() const { return m_params; }

        /**
         * @brief Get body of the function.
         * @return a @c Statement representing body of the function.
         */
        const Statement* GetBody() const { return m_body; }

        static FunctionDeclaration*
        Create(Context& context, SourcePosition def, Identifier name, Parameters* param, Statement* body);
    }; // end class FunctionDeclaration

    /**
     * @brief Represents class declaration.
     */
    class ClassDeclaration final : public Declaration, TrailObjects<ClassDeclaration, Declaration*> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = DeclarationKind::Class;

    private:
        SourcePosition m_class;
        Identifier m_name;
        SourcePosition m_extends;
        Identifier m_extendName;
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;

        ClassDeclaration(SourceRange range, SourcePosition _class, Identifier name, SourcePosition extends,
                         Identifier extendName, SourcePosition open, SourcePosition close, size_t count)
            : Declaration(Kind, range), m_class(_class), m_name(std::move(name)), m_extends(extends),
              m_extendName(std::move(extendName)), m_open(open), m_close(close), m_count(count) {}

        size_t GetTrailCount(OverloadToken<Declaration*>) const { return m_count; } // TrailObjects support.

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'class' keyword.
         * @return a @c SourcePosition representing where 'class' keyword positioned.
         */
        SourcePosition GetClassPosition() const { return m_class; }

        /**
         * @brief Get name of the class.
         * @return an @c Identifier representing name of the class.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief
         * @return
         */
        bool HasExtends() const { return (bool)m_extends; }

        /**
         * @brief
         * @return
         */
        SourcePosition GetExtendsPosition() const { return m_extends; }

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

        static ClassDeclaration*
        Create(Context& context, SourcePosition _class, Identifier name, SourcePosition extends, Identifier extendName,
               SourcePosition open, const std::vector<Declaration*>& nodes, SourcePosition close);
    }; // end class ClassDeclaration

    /**
     * @brief Represents task declaration.
     */
    class TaskDeclaration final : public Declaration,
                                  TrailObjects<TaskDeclaration, Declaration*, Identifier, SourcePosition> {
        friend TrailObjects;

    public:
        static constexpr auto Kind = DeclarationKind::Task;

    private:
        SourcePosition m_task;
        Identifier m_name;
        SourcePosition m_extends;
        Identifier m_extendName;
        SourcePosition m_dependsOn;
        SourcePosition m_open;
        SourcePosition m_close;
        size_t m_count;
        size_t m_depsCount;

        TaskDeclaration(SourceRange range, SourcePosition task, Identifier name, SourcePosition extends,
                        Identifier extendName, SourcePosition dependsOn, SourcePosition open, SourcePosition close,
                        size_t count, size_t depsCount)
            : Declaration(Kind, range), m_task(task), m_name(std::move(name)), m_extends(extends),
              m_extendName(std::move(extendName)), m_dependsOn(dependsOn), m_open(open), m_close(close), m_count(count),
              m_depsCount(depsCount) {}

        size_t GetTrailCount(OverloadToken<Declaration*>) const { return m_count; } // TrailObjects support.
        size_t GetTrailCount(OverloadToken<Identifier>) const { return m_depsCount; }
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return (m_depsCount == 0) ? 0 : (m_depsCount - 1); }

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'task' keyword.
         * @return a @c SourcePosition representing where 'task' keyword positioned.
         */
        SourcePosition GetTaskPosition() const { return m_task; }

        /**
         * @brief Get name of the task.
         * @return an @c Identifier representing name of the task.
         */
        const Identifier& GetName() const { return m_name; }

        bool HasExtends() const { return (bool)m_extends; }

        SourcePosition GetExtendsPosition() const { return m_extends; }

        const Identifier& GetExtendName() const { return m_extendName; }

        bool HasDependsOn() const { return (bool)m_dependsOn; }

        SourcePosition GetDependsOnPosition() const { return m_dependsOn; }

        TrailIterator<const Identifier> GetDependencyNames() const { return GetTrailObjects<Identifier>(); }

        const Identifier& GetDependecyNameAt(size_t index) const { return At<Identifier>(index); }

        SourcePosition GetCommaPositionAt(size_t index) const { return At<SourcePosition>(index); }

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

        static TaskDeclaration*
        Create(Context& context, SourcePosition task, Identifier name, SourcePosition extends, Identifier extendName,
               SourcePosition dependsOn, const std::vector<Identifier>& depnames,
               const std::vector<SourcePosition>& commas, SourcePosition open, const std::vector<Declaration*>& nodes,
               SourcePosition close);
    }; // end class TaskDeclaration

    /**
     * @brief
     */
    enum class SpecifierKind {
        Var,        //!< Variable is writable.
        Const,      //!< Variable or field is readonly.
        Static      //!< Field is static.
    }; // SpecifierKind

    /**
     * @brief Represents variable declaration.
     */
    class VariableDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::Variable;

    private:
        SourcePosition m_keyword;
        SpecifierKind m_kind;
        Identifier m_name;
        SourcePosition m_assign;
        Expression* m_value;

        VariableDeclaration(SourceRange range, SourcePosition key, SpecifierKind kind, Identifier name,
                            SourcePosition assign, Expression* value)
            : Declaration(Kind, range), m_keyword(key), m_kind(kind), m_name(std::move(name)), m_assign(assign),
              m_value(value) {
            assert((m_kind !=  SpecifierKind::Static) && "kind of variable cannot be 'static'");
        }

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'var' keyword.
         * @return a @c SourcePosition representing where 'var' keyword positioned.
         * @note Maybe empty if variable is const.
         */
        SourcePosition GetVarPosition() const {
            return (m_kind == SpecifierKind::Var) ? SourcePosition::Empty() : m_keyword;
        }

        /**
         * @brief Get a position of 'const' keyword.
         * @return a @c SourcePosition representing where 'const' keyword positioned.
         * @note Maybe empty if variable is not const.
         */
        SourcePosition GetConstPosition() const {
            return (m_kind == SpecifierKind::Const) ? m_keyword : SourcePosition::Empty();
        }

        /**
         * @brief
         * @return
         */
        bool IsConst() const { return (m_kind == SpecifierKind::Const); }

        /**
         * @brief Get name of the variable.
         * @return an @c Identifier representing name of the variable.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief Get a position of '='.
         * @return a @c SourcePosition representing where '=' positioned.
         */
        SourcePosition GetAssignPosition() const { return m_assign; }

        /**
         * @brief Get an expression represents initial value of the declaration.
         * @return an @c Expression representing initial value.
         */
        const Expression* GetValue() const { return m_value; }

        static VariableDeclaration*
        Create(Context& context, SourcePosition _const, SourcePosition var, Identifier name, SourcePosition assign,
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

        TaskInputsDeclaration(SourceRange range, SourcePosition inputs, Expression* inputsValue, SourcePosition with,
                              Expression* withValue)
            : Declaration(Kind, range), m_inputs(inputs), m_inputsValue(inputsValue), m_with(with),
              m_withValue(withValue) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'inputs' keyword.
         * @return a @c SourcePosition representing where 'inputs' keyword positioned.
         */
        SourcePosition GetInputsPosition() const { return m_inputs; }

        const Expression* GetInputsValue() const { return m_inputsValue; }

        /**
         * @brief Get a position of 'with' keyword.
         * @return a @c SourcePosition representing where 'with' keyword positioned.
         */
        SourcePosition GetWithPosition() const { return m_with; }

        const Expression* GetWithValue() const { return m_withValue; }

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

        TaskOutputsDeclaration(SourceRange range, SourcePosition outputs, Expression* outputsValue, SourcePosition from,
                               Expression* fromValue)
            : Declaration(Kind, range), m_outputs(outputs), m_outputsValue(outputsValue), m_from(from),
              m_fromValue(fromValue) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'outputs' keyword.
         * @return a @c SourcePosition representing where 'outputs' keyword positioned.
         */
        SourcePosition GetOutputsPosition() const { return m_outputs; }

        const Expression* GetOutputsValue() const { return m_outputsValue; }

        /**
         * @brief Get a position of 'from' keyword.
         * @return a @c SourcePosition representing where 'from' keyword positioned.
         */
        SourcePosition GetFromPosition() const { return m_from; }

        const Expression* GetFromValue() const { return m_fromValue; }

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

        TaskActionDeclaration(SourceRange range, ActionKind kind, SourcePosition pos, Statement* body)
            : Declaration(Kind, range), m_kind(kind), m_pos(pos), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

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
        const Statement* GetBody() const { return m_body; }

        static TaskActionDeclaration* Create(Context& context, ActionKind kind, SourcePosition pos, Statement* body);
    }; // end class TaskActionDeclaration

    /**
     * @brief Represents property in task declaration.
     */
    class TaskPropertyDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::TaskProperty;

    private:
        Identifier m_name;
        SourcePosition m_assign;
        Expression* m_value;

        TaskPropertyDeclaration(SourceRange range, Identifier name, SourcePosition assign, Expression* value)
            : Declaration(Kind, range), m_name(std::move(name)), m_assign(assign), m_value(value) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get name of the property.
         * @return an @c Identifier representing name of the property.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief Get a position of '='.
         * @return a @c SourcePosition representing where '=' positioned.
         */
        SourcePosition GetAssignPosition() const { return m_assign; }

        const Expression* GetValue() const { return m_value; }

        static TaskPropertyDeclaration*
        Create(Context& context, Identifier name, SourcePosition assign, Expression* value);
    }; // end class TaskPropertyDeclaration

    //
    // Class members
    //
    /**
     * @brief Represents initializer in class declaration.
     */
    class ClassInitDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassInit;

    private:
        SourcePosition m_init;
        Parameters* m_params;
        Statement* m_body;

        ClassInitDeclaration(SourceRange range, SourcePosition init, Parameters* params, Statement* body)
            : Declaration(Kind, range), m_init(init), m_params(params), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'init' keyword.
         * @return a @c SourcePosition representing where 'init' keyword positioned.
         */
        SourcePosition GetInitPosition() const { return m_init; }

        /**
         * @brief Get parameters of the initializer.
         * @return @c Parameters representing parameters of the initializer.
         */
        const Parameters* GetParameters() const { return m_params; }

        /**
         * @brief Get body of the initializer.
         * @return a @c Statement representing body of the initializer.
         */
        const Statement* GetBody() const { return m_body; }

        static ClassInitDeclaration* Create(Context& context, SourcePosition init, Parameters* params, Statement* body);
    }; // end class ClassInitDeclaration

    /**
     * @brief Represent deinitializer in class declaration.
     */
    class ClassDeinitDeclaration final : public Declaration  {
    public:
        static constexpr auto Kind = DeclarationKind::ClassDeinit;

    private:
        SourcePosition m_deinit;
        Statement* m_body;

        ClassDeinitDeclaration(SourceRange range, SourcePosition deinit, Statement* body)
            : Declaration(Kind, range), m_deinit(deinit), m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

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
        const Statement* GetBody() const { return m_body; }

        static ClassDeinitDeclaration* Create(Context& context, SourcePosition deinit, Statement* body);
    }; // end class ClassDeinitDeclaration

    /**
     * @brief Represents field in class declaration.
     */
    class ClassFieldDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassField;

    private:
        SourcePosition m_keyword;
        SpecifierKind m_kind;
        Identifier m_name;
        SourcePosition m_assign;
        Expression* m_value;

        ClassFieldDeclaration(SourceRange range, SourcePosition keyword, SpecifierKind kind, Identifier name,
                              SourcePosition assign, Expression* value)
            : Declaration(Kind, range), m_keyword(keyword), m_kind(kind), m_name(std::move(name)), m_assign(assign),
              m_value(value) {
            assert((m_kind != SpecifierKind::Var) && "kind of field cannot be 'var'.");
        }

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        bool IsConst() const { return (m_kind == SpecifierKind::Const); }

        bool IsStatic() const { return (m_kind == SpecifierKind::Static); }

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
         * @brief Get name of the field.
         * @return an @c Identifier representing name of the field.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief Get a position of '='.
         * @return a @c SourcePosition representing where '=' positioned.
         */
        SourcePosition GetAssignPosition() const { return m_assign; }

        const Expression* GetValue() const { return m_value; }

        static ClassFieldDeclaration*
        Create(Context& context, SourcePosition _const, SourcePosition _static, Identifier name, SourcePosition assign,
               Expression* value);
    }; // end class ClassFieldDeclaration

    /**
     * @brief Represents method in class declaration.
     */
    class ClassMethodDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassMethod;

    private:
        SourcePosition m_static;
        SourcePosition m_def;
        Identifier m_name;
        Parameters* m_params;
        Statement* m_body;

        ClassMethodDeclaration(SourceRange range, SourcePosition _static, SourcePosition def, Identifier name,
                               Parameters* params, Statement* body)
            : Declaration(Kind, range), m_static(_static), m_def(def), m_name(std::move(name)), m_params(params),
              m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
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
         * @brief Get name of the method.
         * @return an @c Identifier representing name of the method.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief Get parameters of the method.
         * @return @c Parameters representing parameters of the method.
         */
        const Parameters* GetParameters() const { return m_params; }

        /**
         * @brief Get body of the method.
         * @return a @c Statement representing body of the method.
         */
        const Statement* GetBody() const { return m_body; }

        static ClassMethodDeclaration*
        Create(Context& context, SourcePosition _static, SourcePosition def, Identifier name, Parameters* params,
               Statement* body);
    }; // end class ClassMethodDeclaration

    /**
     * @brief Represents property(getter or setter) in class declaration.
     */
    class ClassPropertyDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassProperty;

    private:
        SourcePosition m_keyword;
        Identifier m_name;
        bool m_isGetter;
        Statement* m_body;

        ClassPropertyDeclaration(SourceRange range, SourcePosition keyword, Identifier name, bool isGetter,
                                 Statement* body)
            : Declaration(Kind, range), m_keyword(keyword), m_name(std::move(name)), m_isGetter(isGetter),
              m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        bool IsGetter() const { return m_isGetter; }

        bool IsSetter() const { return !m_isGetter; }

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
         * @brief Get name of the property.
         * @return an @c Identifier representing name of the property.
         */
        const Identifier& GetName() const { return m_name; }

        /**
         * @brief Get body of the property.
         * @return a @c Statement representing body of the property.
         */
        const Statement* GetBody() const { return m_body; }

        static ClassPropertyDeclaration*
        Create(Context& context, SourcePosition get, SourcePosition set, Identifier name, Statement* body);
    }; // end class ClassPropertyDeclaration

    /**
     * @brief Represents operator overload in class declaration.
     */
    class ClassOperatorDeclaration final : public Declaration {
    public:
        static constexpr auto Kind = DeclarationKind::ClassOperator;

    private:
        SourcePosition m_operator;
        OperatorKind m_kind;
        std::array<SourcePosition, 2> m_pos;
        Parameters* m_params;
        Statement* m_body;

        ClassOperatorDeclaration(SourceRange range, SourcePosition _operator, OperatorKind kind,
                                 std::array<SourcePosition, 2> pos, Parameters* params, Statement* body)
            : Declaration(Kind, range), m_operator(_operator), m_kind(kind), m_pos(pos), m_params(params),
              m_body(body) {}

        const ASTNode* GetChild(size_t index) const override; // ASTIterator support.

    public:
        /**
         * @brief Get a position of 'operator' keyword.
         * @return a @c SourcePosition representing where 'operator' keyword positioned.
         */
        SourcePosition GetOperatorPosition() const { return m_operator; }

        /**
         * @brief
         * @return
         */
        OperatorKind GetOperatorKind() const { return m_kind; }

        /**
         * @brief Get a position of first operator.
         * @return a @c SourcePosition representing where first operator positioned.
         */
        SourcePosition GetFirstOperatorPosition() const { return m_pos[0]; }

        /**
         * @brief Get a position of second operator.
         * @return a @c SourcePosition representing where second operator positioned.
         */
        SourcePosition GetSecondOperatorPosition() const { return m_pos[1]; }

        /**
         * @brief Get parameters of the operator.
         * @return @c Parameters representing parameters of the operator.
         */
        const Parameters* GetParameters() const { return m_params; }

        /**
         * @brief Get body of the operator.
         * @return a @c Statement representing body of the operator.
         */
        const Statement* GetBody() const { return m_body; }

        static ClassOperatorDeclaration*
        Create(Context& context, SourcePosition _operator, OperatorKind kind, std::array<SourcePosition, 2> pos,
               Parameters* param, Statement* body);
    }; // end class ClassOperatorDeclaration
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_DECLARATIONS_H