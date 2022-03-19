/*
 * ASTDumper.cpp
 * - Print AST to stdout.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/ASTDumper.h>

#include <iostream>

#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/AST/Parameters.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/SourceText.h>

using namespace BuildScript;

struct {} Indent;
struct {} Dedent;
struct {} EOL;

class ASTDumper::ASTWriter {
private:
    unsigned m_level = 0;
    bool m_newline = false;

    void WriteString(std::string_view str) {
        if (m_newline) {
            m_newline = false;

            std::cout << "\n" << std::string(m_level * 4, ' ');
        }

        std::cout << str;
    }

public:
    ASTWriter& operator <<(int i) {
        WriteString(std::to_string(i));
        return *this;
    }

    ASTWriter& operator <<(int64_t i) {
        WriteString(std::to_string(i));
        return *this;
    }

    ASTWriter& operator <<(unsigned d) {
        WriteString(std::to_string(d));
        return *this;
    }

    ASTWriter& operator <<(size_t d) {
        WriteString(std::to_string(d));
        return *this;
    }

    ASTWriter& operator <<(double d) {
        WriteString(std::to_string(d));
        return *this;
    }

    ASTWriter& operator <<(bool b) {
        WriteString(b ? "true" : "false");
        return *this;
    }

    ASTWriter& operator <<(const char* str) {
        WriteString(str);
        return *this;
    }

    ASTWriter& operator <<(std::string_view str) {
        WriteString(str);
        return *this;
    }

    ASTWriter& operator <<(const SourcePosition& pos) {
        if (pos.IsValid()) {
            *this << pos.Line << ":" << pos.Column << "(" << pos.Cursor << ")";
        }
        else {
            *this << "< Invalid Position >";
        }
        return *this;
    }

    ASTWriter& operator <<(const SourceRange& range) {
        if (range.IsValid()) {
            *this << range.Begin << " ~ " << range.End;
        }
        else {
            *this << "< Invalid Range >";
        }

        return *this;
    }

    ASTWriter& operator <<(const Identifier& id) {
        *this << "\"" << id.GetString() << "\"(" << id.GetRange() << ")";
        return *this;
    }

    ASTWriter& operator <<(BinaryOp op) {
        static const char* const table[] = {
            "Mul",
            "Div",
            "Mod",
            "Add",
            "Sub",
            "LeftShift",
            "RightShift",
            "BitAnd",
            "BitXor",
            "BitOr",
            "Less",
            "LessOrEqual",
            "Grater",
            "GraterOrEqual",
            "Is",
            "IsNot",
            "In",
            "NotIn",
            "Equal",
            "NotEqual",
            "LogicalAnd",
            "LogicalOr"
        };

        *this << table[static_cast<size_t>(op)];

        return *this;
    }

    ASTWriter& operator <<(UnaryOp op) {
        static const char* const table[] = {
            "Identity",
            "Negate",
            "LogicalNot",
            "BinaryNot"
        };

        *this << table[static_cast<size_t>(op)];

        return *this;
    }

    ASTWriter& operator <<(AssignOp op) {
        static const char* const table[] = {
            "Assign",
            "Add",
            "Sub",
            "Mul",
            "Div",
            "Mod",
            "BitAnd",
            "BitOr",
            "BitXor",
            "LeftShift",
            "RightShift"
        };

        *this << table[static_cast<size_t>(op)];

        return *this;
    }

    ASTWriter& operator <<(OperatorKind kind) {
        *this << OperatorKindToString(kind);
        return *this;
    }

    ASTWriter& operator <<(LiteralType type) {
        static const char* const table[] = {
            "None",
            "Variable",
            "Self",
            "Super",
            "Integer",
            "Float",
            "Boolean",
            "String"
        };

        *this << table[static_cast<size_t>(type)];

        return *this;
    }

    ASTWriter& operator <<(const decltype(Indent)&) {
        m_level++;
        return *this;
    }

    ASTWriter& operator <<(const decltype(Dedent)&) {
        if (m_level > 0) {
            m_level--;
        }

        return *this;
    }

    ASTWriter& operator <<(const decltype(EOL)&) {
        m_newline = true;
        return *this;
    }
};

void ASTDumper::Dump(ASTNode* root) {
    ASTWriter W;

    m_writer = &W;

    super::Walk(root);

    std::cout << std::endl;
}

void ASTDumper::Walk(Parameters* node) {
    writer() << "<< Parameters >>" << EOL;

    writer() << Indent;
    {
        writer() << "- OpenParenPosition: " << node->GetOpenParenPosition() << EOL;
        writer() << "- CloseParenPosition: " << node->GetCloseParenPosition() << EOL;
        writer() << "- HasVariadicArgs: " << node->HasVariadicArgument() << EOL;
        writer() << "- Parameters:" << EOL << Indent;

        auto index = 0;
        for (auto& name : node->GetParameterNames()) {
            writer() << "<< Parameter #" << index++ << " >>" << EOL << Indent;
            {
                writer() << "- Name: " << name << EOL;
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

// Declarations
void ASTDumper::Walk(InvalidDeclaration* node) {
    writer() << "<< InvalidDeclaration >>" << EOL << Indent;
    {
        writer() << "- Range: " << node->GetRange() << EOL;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ScriptDeclaration* node) {
    writer() << "<< ScriptNode >>" << EOL << Indent;
    {
        writer() << "- Name: " << node->GetName() << EOL;
        writer() << "- Nodes:" << EOL << Indent;
        {
            super::Walk(node);
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TaskInputsDeclaration* node) {
    writer() << "<< TaskInputsDeclaration >>" << EOL << Indent;
    {
        writer() << "- InputsKeywordPosition: " << node->GetInputsPosition() << EOL;
        writer() << "- WithKeywordPosition: " << node->GetWithPosition() << EOL;
        writer() << "- Inputs:" << EOL << Indent;
        {
            super::Walk(node->GetInputsValue());
        }
        writer() << Dedent;
        if (node->HasWith()) {
            writer() << "- With:" << EOL << Indent;
            {
                super::Walk(node->GetWithValue());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TaskOutputsDeclaration* node) {
    writer() << "<< TaskOutputsDeclaration >>" << EOL << Indent;
    {
        writer() << "- OutputsKeywordPosition: " << node->GetOutputsPosition() << EOL;
        writer() << "- FromKeywordPosition: " << node->GetFromPosition() << EOL;
        writer() << "- Outputs:" << EOL << Indent;
        {
            super::Walk(node->GetOutputsValue());
        }
        writer() << Dedent;
        if (node->HasFrom()) {
            writer() << "- From:" << EOL << Indent;
            {
                super::Walk(node->GetFromValue());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TaskActionDeclaration* node) {
    writer() << "<< TaskActionDeclaration >>" << EOL << Indent;
    {
        writer() << "- KeywordPosition: " << node->GetKeywordPosition() << EOL;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TaskPropertyDeclaration* node) {
    writer() << "<< TaskPropertyDeclaration >>" << EOL << Indent;
    {
        writer() << "- Name: " << node->GetName() << EOL;
        writer() << "- AssignPosition: " << node->GetAssignPosition() << EOL;
        writer() << "- Value:" << EOL << Indent;
        {
            super::Walk(node->GetValue());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TaskDeclaration* node) {
    writer() << "<< TaskDeclaration >>" << EOL << Indent;
    {
        writer() << "- TaskKeywordPosition: " << node->GetTaskPosition() << EOL;
        writer() << "- Name: " << node->GetName() << EOL;

        if (node->HasExtends()) {
            writer() << "- ExtendsKeywordPosition: " << node->GetExtendsPosition() << EOL;
            writer() << "- ExtendsName: " << node->GetExtendName() << EOL;
        }

        if (node->HasDependsOn()) {
            writer() << "- DependsOnKeywordPosition: " << node->GetDependsOnPosition() << EOL;
            writer() << "- Dependencies: " << EOL << Indent;
            {
                auto index = 0;
                for (auto& name: node->GetDependencyNames()) {
                    writer() << " - Dependency #" << index << ": " << name << EOL;
                    ++index;
                }
            }
            writer() << Dedent;
        }

        writer() << "- OpenBracePosition: " << node->GetOpenBracePosition() << EOL;
        writer() << "- CloseBracePosition: " << node->GetCloseBracePosition() << EOL;
        writer() << "- Members:" << EOL << Indent;
        {
            super::Walk(node);
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ClassDeclaration* node) {
    writer() << "<< ClassDeclaration >>" << EOL << Indent;
    {
        writer() << "- ClassKeywordPosition: " << node->GetClassPosition() << EOL;
        writer() << "- Name: " << node->GetName() << EOL;

        if (node->HasExtends()) {
            writer() << "- ExtendsKeywordPosition: " << node->GetExtendsPosition() << EOL;
            writer() << "- ExtendsName: " << node->GetExtendName() << EOL;
        }

        writer() << "- OpenBracePosition: " << node->GetOpenBracePosition() << EOL;
        writer() << "- CloseBracePosition: " << node->GetCloseBracePosition() << EOL;
        writer() << "- Members:" << EOL << Indent;
        {
            super::Walk(node);
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ClassInitDeclaration* node) {
    writer() << "<< ClassInitDeclaration >>" << EOL << Indent;
    {
        writer() << "- InitKeywordPosition: " << node->GetInitPosition() << EOL;
        writer() << "- Parameters:" << EOL << Indent;
        {
            super::Walk(node->GetParameters());
        }
        writer() << Dedent;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ClassDeinitDeclaration* node) {
    writer() << "<< ClassDeinitDeclaration >>" << EOL << Indent;
    {
        writer() << "- DeinitKeywordPosition: " << node->GetDeinitPosition() << EOL;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ClassFieldDeclaration* node) {
    writer() << "<< ClassFieldDeclaration >>" << EOL << Indent;
    {
        if (node->IsStatic()) {
            writer() << "- StaticKeywordPosition: " << node->GetStaticPosition() << EOL;
        }
        else {
            writer() << "- ConstKeywordPosition: " << node->GetConstPosition() << EOL;
        }
        writer() << "- Name: " << node->GetName() << EOL;
        writer() << "- AssignPosition: " << node->GetAssignPosition() << EOL;
        writer() << "- Value:" << EOL << Indent;
        {
            super::Walk(node->GetValue());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ClassMethodDeclaration* node) {
    writer() << "<< ClassMethodDeclaration >>" << EOL << Indent;
    {
        writer() << "- DefKeywordPosition: " << node->GetDefPosition() << EOL;
        if (node->IsStatic()) {
            writer() << "- StaticKeywordPosition: " << node->GetStaticPosition() << EOL;
        }
        if (node->IsOperator()) {
            writer() << "- Operator: " << node->GetOperator() << EOL;
            writer() << "- OperatorPosition: " << node->GetOperatorPosition() << EOL;
        }
        else {
            writer() << "- Name: " << node->GetName() << EOL;
        }
        writer() << "- Parameters:" << EOL << Indent;
        {
            super::Walk(node->GetParameters());
        }
        writer() << Dedent;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ClassPropertyDeclaration* node) {
    writer() << "<< ClassPropertyDeclaration >>" << EOL << Indent;
    {
        if (node->IsGetter()) {
            writer() << "- GetKeywordPosition: " << node->GetGetPosition() << EOL;
        }
        else {
            writer() << "- SetKeywordPosition: " << node->GetSetPosition() << EOL;
        }
        if (node->IsSubscript()) {
            writer() << "- SubscriptKeywordPosition: " << node->GetSubscriptPosition() << EOL;
        }
        else {
            writer() << "- Name: " << node->GetName() << EOL;
        }
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(FunctionDeclaration* node) {
    writer() << "<< FunctionDeclaration >>" << EOL << Indent;
    {
        writer() << "- DefKeywordPosition: " << node->GetDefPosition() << EOL;
        writer() << "- Name: " << node->GetName() << EOL;
        writer() << "- Parameters:" << EOL << Indent;
        {
            super::Walk(node->GetParameters());
        }
        writer() << Dedent;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ImportDeclaration* node) {
    writer() << "<< ImportDeclaration >>" << EOL << Indent;
    {
        writer() << "- ImportKeywordPosition: " << node->GetImportPosition() << EOL;
        writer() << "- Path:" << EOL << Indent;
        {
            super::Walk(node->GetPath());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ExportDeclaration* node) {
    writer() << "<< ExportDeclaration >>" << EOL << Indent;
    {
        writer() << "- ExportKeywordPosition: " << node->GetExportPosition() << EOL;
        writer() << "- Name: " << node->GetName() << EOL;
        if (node->HasValue()) {
            writer() << "- AssignPosition: " << node->GetAssignPosition() << EOL;
            writer() << "- Value:" << EOL << Indent;
            {
                super::Walk(node->GetValue());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(VariableDeclaration* node) {
    writer() << "<< VariableDeclaration >>" << EOL << Indent;
    {
        if (node->IsConst()) {
            writer() << "- ConstKeywordPosition: " << node->GetConstPosition() << EOL;
        }
        else {
            writer() << "- VarKeywordPosition: " << node->GetVarPosition() << EOL;
        }

        writer() << "- Const: " << node->IsConst() << EOL;
        writer() << "- Name: " << node->GetName() << EOL;
        writer() << "- AssignPosition: " << node->GetAssignPosition() << EOL;
        writer() << "- Value:" << EOL << Indent;
        {
            super::Walk(node->GetValue());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

// Statements
void ASTDumper::Walk(InvalidStatement* node) {
    writer() << "<< InvalidStatement >>" << EOL << Indent;
    {
        writer() << "- Range: " << node->GetRange() << EOL;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ArrowStatement* node) {
    writer() << "<< ArrowStatement >>" << EOL << Indent;
    {
        writer() << "- ArrowPosition: " << node->GetArrowPosition() << EOL;
        writer() << "- Expression:" << EOL << Indent;
        {
            super::Walk(node->GetExpression());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(BlockStatement* node) {
    writer() << "<< BlockStatement >>" << EOL << Indent;
    {
        writer() << "- OpenBrace: " << node->GetOpenBracePosition() << EOL;
        writer() << "- CloseBrace: " << node->GetCloseBracePosition() << EOL;
        writer() << "- Nodes:" << EOL << Indent;
        {
            super::Walk(node);
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(IfStatement* node) {
    writer() << "<< IfStatement >>" << EOL << Indent;
    {
        writer() << "- IfKeyword: " << node->GetIfPosition() << EOL;
        writer() << "- ElseKeyword: " << node->GetElsePosition() << EOL;
        writer() << "- Condition:" << EOL << Indent;
        {
            super::Walk(node->GetCondition());
        }
        writer() << Dedent;
        writer() << "- IfBlock:" << EOL << Indent;
        {
            super::Walk(node->GetIfBody());
        }
        writer() << Dedent;
        if (node->HasElse()) {
            writer() << "- ElseBlock:" << EOL << Indent;
            {
                super::Walk(node->GetElseBody());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(Label* node) {
    writer() << "<< Label >>" << EOL << Indent;
    {
        if (node->IsDefault()) {
            writer() << "- DefaultKeywordPosition: " << node->GetDefaultPosition() << EOL;
            writer() << "- ColonPosition: " << node->GetColonPosition() << EOL;
        }
        else {
            writer() << "- CaseKeywordPosition: " << node->GetCasePosition() << EOL;
            writer() << "- ColonPosition: " << node->GetColonPosition() << EOL;
            writer() << "- Value:" << EOL << Indent;
            {
                super::Walk(node->GetCaseValue());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(LabeledStatement* node) {
    writer() << "<< LabeledStatement >>" << EOL << Indent;
    {
        writer() << "- Labels:" << EOL << Indent;
        {
            for (auto* label : node->GetLabels()) {
                Walk(label);
            }
        }
        writer() << Dedent;
        writer() << "- Nodes:" << EOL << Indent;
        {
            for (auto* e : node->GetNodes()) {
                super::Walk(e);
            }
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(MatchStatement* node) {
    writer() << "<< MatchStatement >>" << EOL << Indent;
    {
        writer() << "- MatchKeywordPosition: " << node->GetMatchPosition() << EOL;
        writer() << "- Condition:" << EOL << Indent;
        {
            super::Walk(node->GetCondition());
        }
        writer() << Dedent;
        writer() << "- Statements:" << EOL << Indent;
        {
            for (auto* stmt : node->GetStatements()) {
                super::Walk(stmt);
            }
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ForStatement* node) {
    writer() << "<< ForStatement >>" << EOL << Indent;
    {
        writer() << "- ForKeywordPosition: " << node->GetForPosition() << EOL;
        writer() << "- Parameter:" << node->GetParameterName() << EOL;
        writer() << "- InKeywordPosition: " << node->GetInPosition() << EOL;
        writer() << "- Expression:" << EOL << Indent;
        {
            super::Walk(node->GetExpression());
        }
        writer() << Dedent;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(WhileStatement* node) {
    writer() << "<< WhileStatement >>" << EOL << Indent;
    {
        writer() << "- WhileKeywordPosition: " << node->GetWhilePosition() << EOL;
        writer() << "- Condition:" << EOL << Indent;
        {
            super::Walk(node->GetCondition());
        }
        writer() << Dedent;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(WithStatement* node) {
    writer() << "<< WithStatement >>" << EOL << Indent;
    {
        writer() << "- WithKeywordPosition: " << node->GetWithPosition() << EOL;
        if (node->HasCapture()) {
            writer() << "- AsKeywordPosition: " << node->GetAsPosition() << EOL;
            writer() << "- CaptureName: " << node->GetCaptureName() << EOL;
        }
        writer() << "- Expression:" << EOL << Indent;
        {
            super::Walk(node->GetExpression());
        }
        writer() << Dedent;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ExceptStatement* node) {
    writer() << "<< ExceptClause >>" << EOL << Indent;
    {
        writer() << "- ExceptKeywordPosition: " << node->GetExceptPosition() << EOL;
        writer() << "- Typename: " << node->GetTypename() << EOL;

        if (node->HasCapture()) {
            writer() << "- AsKeywordPosition: " << node->GetAsPosition() << EOL;
            writer() << "- CaptureName: " << node->GetCaptureName() << EOL;
        }

        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(FinallyStatement* node) {
    writer() << "<< FinallyClause >>" << EOL << Indent;
    {
        writer() << "- FinallyKeywordPosition: " << node->GetFinallyPosition() << EOL;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TryStatement* node) {
    writer() << "<< TryStatement >>" << EOL << Indent;
    {
        writer() << "- TryKeywordPosition: " << node->GetTryPosition() << EOL;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
        writer() << "- Handlers:" << EOL << Indent;
        {
            for (auto* handler : node->GetHandlers()) {
                super::Walk(handler);
            }
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(BreakStatement* node) {
    writer() << "<< BreakStatement >>" << EOL << Indent;
    {
        writer() << "- BreakKeywordPosition: " << node->GetBreakPosition() << EOL;

        if (node->IsConditional()) {
            writer() << "- IfKeywordPosition: " << node->GetIfPosition() << EOL;
            writer() << "- Condition:" << EOL << Indent;
            {
                super::Walk(node->GetCondition());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ContinueStatement* node) {
    writer() << "<< ContinueStatement >>" << EOL << Indent;
    {
        writer() << "- ContinueKeywordPosition: " << node->GetContinuePosition() << EOL;

        if (node->IsConditional()) {
            writer() << "- IfKeywordPosition: " << node->GetIfPosition() << EOL;
            writer() << "- Condition:" << EOL << Indent;
            {
                super::Walk(node->GetCondition());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ReturnStatement* node) {
    writer() << "<< ReturnStatement >>" << EOL << Indent;
    {
        writer() << "- ReturnKeywordPosition: " << node->GetReturnPosition() << EOL;
        if (node->HasReturnValue()) {
            writer() << "- ReturnValue:" << EOL << Indent;
            {
                super::Walk(node->GetReturnValue());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(AssertStatement* node) {
    writer() << "<< AssertStatement >>" << EOL << Indent;
    {
        writer() << "- AssertKeywordPosition: " << node->GetAssertPosition() << EOL;

        writer() << "- Condition:" << EOL << Indent;
        {
            super::Walk(node->GetCondition());
        }
        writer() << Dedent;
        if (node->HasMessage()) {
            writer() << "- ColonPosition: " << node->GetColonPosition() << EOL;
            writer() << "- Message:" << EOL << Indent;
            {
                super::Walk(node->GetMessage());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(PassStatement* node) {
    writer() << "<< PassStatement >>" << EOL << Indent;
    {
        writer() << "- PassKeywordPosition: " << node->GetPassPosition() << EOL;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(AssignStatement* node) {
    writer() << "<< AssignStatement >>" << EOL << Indent;
    {
        writer() << "- Operation: " << node->GetOp() << EOL;
        writer() << "- OperatorPosition: " << node->GetOpPosition() << EOL;
        writer() << "- Target:" << EOL << Indent;
        {
            super::Walk(node->GetTarget());
        }
        writer() << Dedent;
        writer() << "- Value:" << EOL << Indent;
        {
            super::Walk(node->GetValue());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

// Expressions
void ASTDumper::Walk(InvalidExpression* node) {
    writer() << "<< InvalidExpression >>" << EOL << Indent;
    {
        writer() << "- Range: " << node->GetRange() << EOL;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(PassExpression* node) {
    writer() << "<< PassExpression >>" << EOL << Indent;
    {
        writer() << "- PassKeywordPosition: " << node->GetPassPosition() << EOL;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TernaryExpression* node) {
    writer() << "<< ConditionalExpression >>" << EOL << Indent;
    {
        writer() << "- IfKeywordPosition: " << node->GetIfPosition() << EOL;
        writer() << "- ElseKeywordPosition: " << node->GetElsePosition() << EOL;
        writer() << "- Condition:" << EOL << Indent;
        {
            super::Walk(node->GetCondition());
        }
        writer() << Dedent;
        writer() << "- ValueOnTrue:" << EOL << Indent;
        {
            super::Walk(node->GetValueOnTrue());
        }
        writer() << Dedent;
        writer() << "- ValueOnFalse:" << EOL << Indent;
        {
            super::Walk(node->GetValueOnFalse());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(BinaryExpression* node) {
    writer() << "<< BinaryExpression >>" << EOL << Indent;
    {
        writer() << "- Operation: " << node->GetOp() << EOL;
        writer() << "- OperatorPosition: " << node->GetOpPosition() << EOL;

        writer() << "- Left:" << EOL << Indent;
        {
            super::Walk(node->GetLeft());
        }
        writer() << Dedent;
        writer() << "- Right:" << EOL << Indent;
        {
            super::Walk(node->GetRight());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(TypeTestExpression* node) {
    writer() << "<< TypeTestExpression >>" << EOL << Indent;
    {
        writer() << "- IsPosition: " << node->GetIsPosition() << EOL;
        if (node->IsNegative()) {
            writer() << "- NotPosition: " << node->GetNotPosition() << EOL;
        }
        writer() << "- Typename: " << node->GetTypename() << EOL;
        writer() << "- Target:" << EOL << Indent;
        {
            super::Walk(node->GetTarget());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ContainmentTestExpression* node) {
    writer() << "<< ContainmentTestExpression >>" << EOL << Indent;
    {
        if (node->IsNegative()) {
            writer() << "- NotPosition: " << node->GetNotPosition() << EOL;
        }
        writer() << "- InPosition: " << node->GetInPosition() << EOL;
        writer() << "- Value:" << EOL << Indent;
        {
            super::Walk(node->GetValue());
        }
        writer() << Dedent;
        writer() << "- Target:" << EOL << Indent;
        {
            super::Walk(node->GetTarget());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(UnaryExpression* node) {
    writer() << "<< UnaryExpression >>" << EOL << Indent;
    {
        writer() << "- Operation: " << node->GetOp() << EOL;
        writer() << "- OperatorPosition: " << node->GetOpPosition() << EOL;
        writer() << "- Expression:" << EOL << Indent;
        {
            super::Walk(node->GetExpression());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(DefinedExpression* node) {
    writer() << "<< DefinedExpression >>" << EOL  << Indent;
    {
        writer() << "- DefinedKeywordPosition: " << node->GetDefinedPosition() << EOL;
        writer() << "- ID: " << node->GetID() << EOL;

        if (node->GetInPosition().IsValid()) {
            writer() << "- InKeywordPosition: " << node->GetInPosition() << EOL;
            writer() << "- Target:" << EOL << Indent;
            {
                super::Walk(node->GetTarget());
            }
            writer() << Dedent;
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(RaiseExpression* node) {
    writer() << "<< RaiseExpression >>" << EOL << Indent;
    {
        writer() << "- RaiseKeywordPosition: " << node->GetRaisePosition() << EOL;
        writer() << "- Target:" << EOL << Indent;
        {
            super::Walk(node->GetTarget());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(MemberAccessExpression* node) {
    writer() << "<< MemberAccessExpression >>" << EOL << Indent;
    {
        writer() << "- MemberName: " << node->GetMemberName() << EOL;
        writer() << "- DotPosition: " << node->GetDotPosition() << EOL;
        writer() << "- Target:" << EOL << Indent;
        {
            super::Walk(node->GetTarget());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(SubscriptExpression* node) {
    writer() << "<< SubscriptExpression >>" << EOL << Indent;
    {
        writer() << "- OpenSquarePosition: " << node->GetOpenSquarePosition() << EOL;
        writer() << "- CloseSquarePosition: " << node->GetCloseSquarePosition() << EOL;
        writer() << "- Target:" << EOL << Indent;
        {
            super::Walk(node->GetTarget());
        }
        writer() << Dedent;
        writer() << "- Index:" << EOL << Indent;
        {
            super::Walk(node->GetIndex());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(InvocationExpression* node) {
    writer() << "<< InvocationExpression >>" << EOL << Indent;
    {
        writer() << "- OpenParenPosition: " << node->GetOpenParenPosition() << EOL;
        writer() << "- CloseParenPosition: " << node->GetCloseParenPosition() << EOL;
        writer() << "- Target:" << EOL << Indent;
        {
            super::Walk(node->GetTarget());
        }
        writer() << Dedent;
        writer() << "- Arguments:" << EOL << Indent;
        {
            auto index = 0;
            for (auto* expr : node->GetArguments()) {
                writer() << "- Argument #" << index << ":" << EOL << Indent;
                {
                    super::Walk(expr);
                }
                writer() << Dedent;
            }
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ParenthesizedExpression* node) {
    writer() << "<< ParenthesizedExpression >>" << EOL << Indent;
    {
        writer() << "- OpenParenPosition: " << node->GetOpenParenPosition() << EOL;
        writer() << "- CloseParenPosition: " << node->GetCloseParenPosition() << EOL;
        writer() << "- InnerExpression:" << EOL << Indent;
        {
            super::Walk(node->GetInnerExpression());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}

void ASTDumper::Walk(LiteralExpression* node) {
    writer() << "<< LiteralExpression >>" << EOL << Indent;
    {
        writer() << "- LiteralType: " << node->GetLiteralType() << EOL;
        writer() << "- RawValue: " << m_source.GetString(node->GetRange()) << EOL;

        if (node->GetLiteralType() == LiteralType::String && node->HasInterpolations()) {
            writer() << "- Interpolations:" << EOL << Indent;
            {
                auto index = 0;
                for (auto* expr : node->GetInterpolations()) {
                    writer() << "- Interpolation #" << index << ":" << EOL << Indent;
                    {
                        super::Walk(expr);
                    }
                    writer() << Dedent;
                }
            }
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ListExpression* node) {
    writer() << "<< ListExpression >>" << EOL << Indent;
    {
        writer() << "- OpenSquarePosition: " << node->GetOpenSquarePosition() << EOL;
        writer() << "- CloseSquarePosition: " << node->GetCloseSquarePosition() << EOL;
        writer() << "- Items:" << EOL << Indent;
        {
            auto index = 0;
            for (auto* expr: node->GetItems()) {
                writer() << "- Item #" << index << ":" << EOL << Indent;
                {
                    super::Walk(expr);
                }
                writer() << Dedent;
            }
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(MapExpression* node) {
    writer() << "<< MapExpression >>" << EOL << Indent;
    {
        writer() << "- OpenBracePosition: " << node->GetOpenBracePosition() << EOL;
        writer() << "- CloseBracePosition: " << node->GetCloseBracePosition() << EOL;
        writer() << "- Arguments:" << EOL << Indent;
        {
            auto index = 0;
            for (auto& [key, colon, value] : node->GetItems()) {
                writer() << "- Item #" << index << ":" << EOL << Indent;
                {
                    writer() << "- ColonPosition: " << colon << EOL;
                    writer() << "- Key:" << EOL << Indent;
                    {
                        super::Walk(key);
                    }
                    writer() << Dedent;
                    writer() << "- Value:" << EOL << Indent;
                    {
                        super::Walk(value);
                    }
                    writer() << Dedent;
                }
                writer() << Dedent;
            }
        }
    }
    writer() << Dedent;
}

void ASTDumper::Walk(ClosureExpression* node) {
    writer() << "<< ClosureExpression >>" << EOL << Indent;
    {
        writer() << "- ArrowPosition: " << node->GetArrowPosition() << EOL;
        writer() << "- Parameters:" << EOL << Indent;
        {
            super::Walk(node->GetParameters());
        }
        writer() << Dedent;
        writer() << "- Body:" << EOL << Indent;
        {
            super::Walk(node->GetBody());
        }
        writer() << Dedent;
    }
    writer() << Dedent;
}