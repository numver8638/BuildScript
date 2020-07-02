/*
 * Declarations.h
 * - Represent declarations in AST.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_DECLARATIONS_H
#define BUILDSCRIPT_COMPILER_AST_DECLARATIONS_H

#include <utility>
#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/StringRef.h>

namespace BuildScript {
    class Parameters {
    public:
        using Parameter = std::pair<StringRef, SourcePosition>;

    private:
        std::vector<Parameter> m_params;
        Parameter m_varadic;

    public:
        Parameters(std::vector<Parameter>&& params, Parameter varadic)
            : m_params(std::move(params)), m_varadic(std::move(varadic)) {}

        const std::vector<Parameter>& GetParameters() const { return m_params; }

        bool IsVaradic() const { return m_varadic.second.IsValid(); }

        const Parameter& GetVaradic() const { return m_varadic; }

        void Accept(ASTVisitor*) const;
    }; // end class Parameters

    struct InputsClause {
        SourcePosition Position;
        Expression* Value = nullptr;
        Expression* With = nullptr;

        InputsClause() = default;

        InputsClause(InputsClause&& rhs) noexcept
            : Position(rhs.Position) {
            delete Value;
            delete With;

            Value = rhs.Value;
            With = rhs.With;

            rhs.Value = nullptr;
            rhs.With = nullptr;
        }

        InputsClause(const InputsClause&) = delete;

        ~InputsClause() {
            delete Value;
            delete With;
        }

        explicit operator bool() const { return Position.IsValid(); }
    }; // end struct InputsClause

    struct OutputsClause {
        SourcePosition Position;
        Expression* Value = nullptr;
        Expression* From = nullptr;

        OutputsClause() = default;

        OutputsClause(OutputsClause&& rhs) noexcept
            : Position(rhs.Position) {
            delete Value;
            delete From;

            Value = rhs.Value;
            From = rhs.From;

            rhs.Value = nullptr;
            rhs.From = nullptr;
        }

        OutputsClause(const OutputsClause&) = delete;

        ~OutputsClause() {
            delete Value;
            delete From;
        }

        explicit operator bool() const { return Position.IsValid(); }
    }; // end struct OutputsClause

    struct Property {
        SourcePosition Position;
        StringRef Name;
        Expression* Value = nullptr;

        Property() = default;

        Property(const SourcePosition& pos, const StringRef& name, Expression* value)
            : Position(pos), Name(name), Value(value) {}

        Property(Property&& rhs) noexcept
            : Position(rhs.Position), Name(rhs.Name) {
            delete Value;
            Value = rhs.Value;
            rhs.Value = nullptr;
        }

        Property(const Property&) = delete;

        ~Property() {
            delete Value;
        }
    }; // end struct Property

    class TaskDeclaration final : public Declaration {
    private:
        StringRef m_name;
        StringRef m_extends;
        std::vector<StringRef> m_dependencies;
        InputsClause m_inputs;
        OutputsClause m_outputs;
        std::vector<Property> m_properties;
        Statement* m_do;
        Statement* m_doFirst;
        Statement* m_doLast;

    public:
        TaskDeclaration(const SourcePosition& pos, const StringRef& name, const StringRef& extends,
                        std::vector<StringRef>&& dependencies,
                        InputsClause&& inputs, OutputsClause&& outputs, std::vector<Property>&& properties,
                        Statement* _do, Statement* doFirst, Statement* doLast)
            : Declaration(pos), m_name(name), m_extends(extends), m_dependencies(std::move(dependencies)),
              m_inputs(std::move(inputs)), m_outputs(std::move(outputs)), m_properties(std::move(properties)),
              m_do(_do), m_doFirst(doFirst), m_doLast(doLast) {}

        ~TaskDeclaration() final {
            delete m_do;
            delete m_doFirst;
            delete m_doLast;
        }

        const StringRef& GetName() const { return m_name; }

        const StringRef& GetExtends() const { return m_extends; }

        const std::vector<StringRef>& GetDependencies() const { return m_dependencies; }

        const InputsClause& GetInputsClause() const { return m_inputs; }

        const OutputsClause& GetOuputsClause() const { return m_outputs; }

        const std::vector<Property>& GetProperties() const { return m_properties; }

        const Statement* GetDoBlock() const { return m_do; }

        const Statement* GetDoFirstBlock() const { return m_doFirst; }

        const Statement* GetDoLastBlock() const { return m_doLast; }

        void Accept(ASTVisitor*) const override;
    }; // end class TaskDeclaration

    class ClassMember : public Declaration {
    protected:
        explicit ClassMember(const SourcePosition& pos)
            : Declaration(pos) {}
    };

    class ClassStaticMember final : public ClassMember {
    private:
        StringRef m_name;
        Expression* m_value;

    public:
        ClassStaticMember(const SourcePosition& pos, const StringRef& name, Expression* value)
            : ClassMember(pos), m_name(name), m_value(value) {}

        ~ClassStaticMember() final {
            delete m_value;
        }

        const StringRef& GetName() const { return m_name; }

        const Expression* GetValue() const { return m_value; }

        void Accept(ASTVisitor*) const override;
    };

    class ClassMethod final : public ClassMember {
    private:
        StringRef m_name;
        bool m_isStatic;
        Parameters* m_params;
        Statement* m_body;

    public:
        ClassMethod(const SourcePosition& pos, const StringRef& name, bool isStatic, Parameters* params, Statement* body)
            : ClassMember(pos), m_name(name), m_isStatic(isStatic), m_params(params), m_body(body) {}

        ~ClassMethod() final {
            delete m_params;
            delete m_body;
        }

        const StringRef& GetName() const { return m_name; }

        bool IsStaticMethod() const { return m_isStatic; }

        const Parameters* GetParameters() const { return m_params; }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    };

    class ClassInit final : public ClassMember {
    private:
        Parameters* m_params;
        Statement* m_body;

    public:
        ClassInit(const SourcePosition& pos, Parameters* params, Statement* body)
            : ClassMember(pos), m_params(params), m_body(body) {}

        ~ClassInit() final {
            delete m_params;
            delete m_body;
        }

        const Parameters* GetParameters() const { return m_params; }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    };

    class ClassDeinit final : public ClassMember {
    private:
        Statement* m_body;

    public:
        ClassDeinit(const SourcePosition& pos, Statement* body)
            : ClassMember(pos), m_body(body) {}

        ~ClassDeinit() final {
            delete m_body;
        }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    };

    enum class PropertyType { Getter, Setter };

    class ClassProperty final : public ClassMember {
    private:
        PropertyType m_type;
        StringRef m_name;
        Statement* m_body;

    public:
        ClassProperty(const SourcePosition& pos, PropertyType type, const StringRef& name, Statement* body)
            : ClassMember(pos), m_type(type), m_name(name), m_body(body) {}

        ~ClassProperty() final {
            delete m_body;
        }

        PropertyType GetType() const { return m_type; }

        const StringRef& GetName() const { return m_name; }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    };

    enum class OperatorKind {
        Error,
        Add,
        InplaceAdd,
        Sub,
        InplaceSub,
        Mul,
        InplaceMul,
        Div,
        InplaceDiv,
        Mod,
        InplaceMod,
        BitAnd,
        InplaceBitAnd,
        BitOr,
        InplaceBitOr,
        BitNot,
        BitXor,
        InplaceBitXor,
        Less,
        LessOrEqual,
        LeftShift,
        InplaceLeftShift,
        Grater,
        GraterOrEqual,
        RightShift,
        InplaceRightShift,
        Equal,
        NotEqual,
        Index,
    };

    class ClassOperator final : public ClassMember {
    private:
        OperatorKind m_kind;
        Parameters* m_params;
        Statement* m_body;

    public:
        ClassOperator(const SourcePosition& pos, OperatorKind kind, Parameters* params, Statement* body)
            : ClassMember(pos), m_kind(kind), m_params(params), m_body(body) {}

        ~ClassOperator() final {
            delete m_params;
            delete m_body;
        }

        OperatorKind GetOperator() const { return m_kind; }

        const Parameters* GetParameters() const { return m_params; }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    };

    class ClassDeclaration final : public Declaration {
    private:
        StringRef m_name;
        StringRef m_extends;
        std::vector<ClassMember*> m_members;

    public:
        ClassDeclaration(const SourcePosition& pos, const StringRef& name, const StringRef& extends,
                         std::vector<ClassMember*>&& members)
            : Declaration(pos), m_name(name), m_extends(extends), m_members(std::move(members)) {}

        ~ClassDeclaration() final {
            for (auto e : m_members)
                delete e;
        }

        const StringRef& GetName() const { return m_name; }

        const StringRef& GetExtends() const { return m_extends; }

        const std::vector<ClassMember*>& GetMembers() const { return m_members; }

        void Accept(ASTVisitor*) const override;
    }; // end class ClassDeclaration

    class FunctionDeclaration final : public Declaration {
    private:
        StringRef m_name;
        Parameters* m_params;
        Statement* m_body;

    public:
        FunctionDeclaration(const SourcePosition& pos, const StringRef& name, Parameters* params, Statement* body)
            : Declaration(pos), m_name(name), m_params(params), m_body(body) {}

        ~FunctionDeclaration() final {
            delete m_params;
            delete m_body;
        }

        const StringRef& GetName() const { return m_name; }

        const Parameters* GetParameters() const { return m_params; }

        const Statement* GetBody() const { return m_body; }

        void Accept(ASTVisitor*) const override;
    }; // end class FunctionDeclaration

    class ImportDeclaration final : public Declaration {
    private:
        std::vector<Expression*> m_imports;
    public:
        ImportDeclaration(const SourcePosition& pos, std::vector<Expression*>&& imports)
            : Declaration(pos), m_imports(std::move(imports)) {}

        ~ImportDeclaration() final {
            for (auto e : m_imports)
                delete e;
        }

        const std::vector<Expression*>& GetImports() const { return m_imports; }

        void Accept(ASTVisitor*) const override;
    }; // end class ImportDeclaration

    class ExportDeclaration final : public Declaration {
    private:
        std::vector<Property> m_exports;

    public:
        ExportDeclaration(const SourcePosition& pos, std::vector<Property>&& exports)
            : Declaration(pos), m_exports(std::move(exports)) {}

        ~ExportDeclaration() final {
            for (auto& e : m_exports)
                delete e.Value;
        }

        const std::vector<Property>& GetExports() const { return m_exports; }

        void Accept(ASTVisitor*) const override;
    }; // end class ExportDeclaration
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_DECLARATIONS_H