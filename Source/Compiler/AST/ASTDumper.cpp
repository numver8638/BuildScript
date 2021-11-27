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

void ASTDumper::Dump(const ASTNode* root) {
    ASTWriter W;

    m_writer = &W;

    Walk(root);

    std::cout << std::endl;
}

WalkerFlags ASTDumper::OnEnterNode(const Parameters& node) {
    writer() << "<< Parameters @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- OpenParenPosition: " << node.GetOpenParenPosition() << EOL;
    writer() << "- CloseParenPosition: " << node.GetCloseParenPosition() << EOL;
    writer() << "- HasVariadicArgs: " << node.HasVariadicArgument() << EOL;
    writer() << "- Parameters:" << EOL << Indent;

    auto index = 0;
    for (const auto& name : node.GetParameterNames()) {
        writer() << "<< Parameter #" << index << " >>" << EOL << Indent;

        writer() << "- Name: " << name << EOL;
        // writer() << "- CommaPosition: " << node.GetCommaPositionAt(index) << EOL;

        ++index;

        writer() << Dedent;
    }

    writer() << Dedent;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const InvalidDeclaration& node) {
    writer() << "<< InvalidDeclaration @ " << node.GetRange() << " >>" << EOL << Indent;
    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ScriptDeclaration& node) {
    writer() << "<< ScriptNode @ " << node.GetRange() << " >>" << EOL << Indent;
    writer() << "- Name: " << node.GetName() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const TaskInputsDeclaration& node) {
    writer() << "<< TaskInputsDeclaration @ " << node.GetRange() << " >>" << EOL << Indent;
    writer() << "- InputsKeywordPosition: " << node.GetInputsPosition() << EOL;
    writer() << "- WithKeywordPosition: " << node.GetWithPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const TaskOutputsDeclaration& node) {
    writer() << "<< TaskOutputsDeclaration @ " << node.GetRange() << " >>" << EOL << Indent;
    writer() << "- OutputsKeywordPosition: " << node.GetOutputsPosition() << EOL;
    writer() << "- FromKeywordPosition: " << node.GetFromPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const TaskActionDeclaration& node) {
    writer() << "<< TaskActionDeclaration @ " << node.GetRange() << " >>" << EOL << Indent;
    writer() << "- KeywordPosition: " << node.GetKeywordPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const TaskPropertyDeclaration& node) {
    writer() << "<< TaskPropertyDeclaration @ " << node.GetRange() << " >>" << EOL << Indent;

    writer() << "- Name: " << node.GetName() << EOL;
    writer() << "- AssignPosition: " << node.GetAssignPosition() << EOL;

    return WalkerFlags::Continue;
}

// Declarations
WalkerFlags ASTDumper::OnEnterNode(const TaskDeclaration& node) {
    writer() << "<< TaskDeclaration @ " << node.GetRange() << " >>" << EOL ;

    writer() << Indent;

    writer() << "- TaskKeywordPosition: " << node.GetTaskPosition() << EOL;
    writer() << "- Name: " << node.GetName() << EOL;

    if (node.HasExtends()) {
        writer() << "- ExtendsKeywordPosition: " << node.GetExtendsPosition() << EOL;
        writer() << "- ExtendsName: " << node.GetExtendName() << EOL;
    }

    if (node.HasDependsOn()) {
        writer() << "- DependsOnKeywordPosition: " << node.GetDependsOnPosition() << EOL;
        writer() << "- Dependencies: " << EOL;

        writer() << Indent;

        auto index = 0;
        for (const auto& name : node.GetDependencyNames()) {
            writer() << " - DepName #" << index << ": " << name << EOL;
            // writer() << " - CommaPosition: " << list->GetCommaPositionAt(index) << EOL;
            ++index;
        }

        writer() << Dedent;
    }

    writer() << "- OpenBracePosition: " << node.GetOpenBracePosition() << EOL;
    writer() << "- CloseBracePosition: " << node.GetCloseBracePosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ClassDeclaration& node) {
    writer() << "<< ClassDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- ClassKeywordPosition: " << node.GetClassPosition() << EOL;
    writer() << "- Name: " << node.GetName() << EOL;

    if (node.HasExtends()) {
        writer() << "- ExtendsKeywordPosition: " << node.GetExtendsPosition() << EOL;
        writer() << "- ExtendsName: " << node.GetExtendName() << EOL;
    }

    writer() << "- OpenBracePosition: " << node.GetOpenBracePosition() << EOL;
    writer() << "- CloseBracePosition: " << node.GetCloseBracePosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const FunctionDeclaration& node) {
    writer() << "<< FunctionDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- DefKeywordPosition: " << node.GetDefPosition() << EOL;
    writer() << "- Name: " << node.GetName() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ImportDeclaration& node) {
    writer() << "<< ImportDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- ImportKeywordPosition: " << node.GetImportPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ExportDeclaration& node) {
    writer() << "<< ExportDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- ExportKeywordPosition: " << node.GetExportPosition() << EOL;
    writer() << "- Name: " << node.GetName() << EOL;
    if (node.HasValue()) {
        writer() << "- AssignPosition: " << node.GetAssignPosition() << EOL;
    }


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const VariableDeclaration& node) {
    writer() << "<< VariableDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    if (node.IsConst()) {
        writer() << "- ConstKeywordPosition: " << node.GetConstPosition() << EOL;
    }
    else {
        writer() << "- VarKeywordPosition: " << node.GetVarPosition() << EOL;
    }

    writer() << "- Const: " << node.IsConst() << EOL;
    writer() << "- Name: " << node.GetName() << EOL;
    writer() << "- AssignPosition: " << node.GetAssignPosition() << EOL;

    return WalkerFlags::Continue;
}

// ClassMembers
WalkerFlags ASTDumper::OnEnterNode(const ClassInitDeclaration& node) {
    writer() << "<< ClassInitDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- InitKeywordPosition: " << node.GetInitPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ClassDeinitDeclaration& node) {
    writer() << "<< ClassDeinitDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- DeinitKeywordPosition: " << node.GetDeinitPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ClassFieldDeclaration& node) {
    writer() << "<< ClassFieldDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- Static: " << node.IsStatic() << EOL;
    writer() << "- Const: " << node.IsConst() << EOL;

    if (node.IsConst()) {
        writer() << "- ConstKeywordPosition: " << node.GetConstPosition() << EOL;
    }

    if (node.IsStatic()) {
        writer() << "- StaticKeywordPosition: " << node.GetStaticPosition() << EOL;
    }

    writer() << "- AssignPosition: " << node.GetAssignPosition() << EOL;
    writer() << "- Name:" << node.GetName() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ClassMethodDeclaration& node) {
    writer() << "<< ClassMethodDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- DefKeywordPosition: " << node.GetDefPosition() << EOL;
    writer() << "- Static: " << node.IsStatic() << EOL;

    if (node.IsStatic()) {
        writer() << "- StaticKeywordPosition: " << node.GetStaticPosition() << EOL;
    }

    writer() << "- Name: " << node.GetName() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ClassPropertyDeclaration& node) {
    writer() << "<< ClassPropertyDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    if (node.IsGetter()) {
        writer() << "- GetKeywordPosition: " << node.GetGetPosition() << EOL;
    }
    else {
        writer() << "- SetKeywordPosition: " << node.GetSetPosition() << EOL;
    }

    writer() << "- Name: " << node.GetName() << EOL;
    writer() << "- Type: " << (node.IsGetter() ? "Getter" : "Setter") << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ClassOperatorDeclaration& node) {
    writer() << "<< ClassOperatorDeclaration @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- OperatorKeywordPosition: " << node.GetOperatorPosition() << EOL;
    writer() << "- OperatorPosition: " << node.GetFirstOperatorPosition() << EOL;

    if (node.GetOperatorKind() == OperatorKind::Index) {
        writer() << "- SecondaryOperatorPosition: " << node.GetSecondOperatorPosition() << EOL;
    }

    writer() << "- OperatorKind: " << node.GetOperatorKind() << EOL;


    return WalkerFlags::Continue;
}

// Statements
WalkerFlags ASTDumper::OnEnterNode(const InvalidStatement& node) {
    writer() << "<< InvalidStatement @ " << node.GetRange() << " >>" << EOL << Indent;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ArrowStatement& node) {
    writer() << "<< ArrowStatement @ " << node.GetRange() << " >>" << EOL;
    writer() << Indent;

    writer() << "- ArrowPosition: " << node.GetArrowPosition() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const BlockStatement& node) {
    writer() << "<< BlockStatement @ " << node.GetRange() << " >>" << EOL;
    writer() << Indent;

    writer() << "- OpenBrace: " << node.GetOpenBracePosition() << EOL;
    writer() << "- CloseBrace: " << node.GetCloseBracePosition() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const IfStatement& node) {
    writer() << "<< IfStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- IfKeyword: " << node.GetIfPosition() << EOL;
    writer() << "- ElseKeyword: " << node.GetElsePosition() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const Label& node) {
    writer() << "<< Label @ " << node.GetRange() << " >>" << EOL << Indent;

    if (node.IsDefault()) {
        writer() << "- DefaultKeywordPosition: " << node.GetDefaultPosition() << EOL;
        writer() << "- ColonPosition: " << node.GetColonPosition() << EOL;
    }
    else {
        writer() << "- CaseKeywordPosition: " << node.GetCasePosition() << EOL;
        writer() << "- ColonPosition: " << node.GetColonPosition() << EOL;
    }


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const LabeledStatement& node) {
    writer() << "<< LabeledStatement @ " << node.GetRange() << " >>" << EOL;
    writer() << Indent;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const MatchStatement& node) {
    writer() << "<< MatchStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- MatchKeywordPosition: " << node.GetMatchPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ForStatement& node) {
    writer() << "<< ForStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- ForKeywordPosition: " << node.GetForPosition() << EOL;
    writer() << "- Parameter:" << node.GetParameterName() << EOL << Indent;

    writer() << Dedent;

    writer() << "- InKeywordPosition: " << node.GetInPosition() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const WhileStatement& node) {
    writer() << "<< WhileStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- WhileKeywordPosition: " << node.GetWhilePosition() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const WithStatement& node) {
    writer() << "<< WithStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- WithKeywordPosition: " << node.GetWithPosition() << EOL;

    if (node.HasCapture()) {
        writer() << "- AsKeywordPosition: " << node.GetAsPosition() << EOL;
        writer() << "- CaptureName: " << node.GetCaptureName() << EOL;
    }

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ExceptStatement& node) {
    writer() << "<< ExceptClause @ " << node.GetRange() << " >>" << EOL << Indent;

    writer() << "- ExceptKeywordPosition: " << node.GetExceptPosition() << EOL;
    writer() << "- Typename: " << node.GetTypename() << EOL;

    if (node.HasCapture()) {
        writer() << "- AsKeywordPosition: " << node.GetAsPosition() << EOL;
        writer() << "- CaptureName: " << node.GetCaptureName() << EOL;
    }

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const FinallyStatement& node) {
    writer() << "<< FinallyClause @ " << node.GetRange() << " >>" << EOL << Indent;

    writer() << "- FinallyKeywordPosition: " << node.GetFinallyPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const TryStatement& node) {
    writer() << "<< TryStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- TryKeywordPosition: " << node.GetTryPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const BreakStatement& node) {
    writer() << "<< BreakStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- BreakKeywordPosition: " << node.GetBreakPosition() << EOL;

    if (node.IsConditional()) {
        writer() << "- IfKeywordPosition: " << node.GetIfPosition() << EOL;
    }

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ContinueStatement& node) {
    writer() << "<< ContinueStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- ContinueKeywordPosition: " << node.GetContinuePosition() << EOL;

    if (node.IsConditional()) {
        writer() << "- IfKeywordPosition: " << node.GetIfPosition() << EOL;
    }

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ReturnStatement& node) {
    writer() << "<< ReturnStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- ReturnKeywordPosition: " << node.GetReturnPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const AssertStatement& node) {
    writer() << "<< AssertStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- AssertKeywordPosition: " << node.GetAssertPosition() << EOL;
    writer() << "- ColonPosition: " << node.GetColonPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const PassStatement& node) {
    writer() << "<< PassStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- PassKeywordPosition: " << node.GetPassPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const AssignStatement& node) {
    writer() << "<< AssignStatement @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- Operation: " << node.GetOp() << EOL;
    writer() << "- OperatorPosition: " << node.GetOpPosition() << EOL;

    return WalkerFlags::Continue;
}

// Expressions
WalkerFlags ASTDumper::OnEnterNode(const InvalidExpression& node) {
    writer() << "<< InvalidExpression @ " << node.GetRange() << " >>" << EOL << Indent;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const PassExpression& node) {
    writer() << "<< PassExpression @ " << node.GetRange() << " >>" << EOL << Indent;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const TernaryExpression& node) {
    writer() << "<< ConditionalExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- IfKeywordPosition: " << node.GetIfPosition() << EOL;
        writer() << "- ElseKeywordPosition: " << node.GetElsePosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const BinaryExpression& node) {
    writer() << "<< BinaryExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- Operation: " << node.GetOp() << EOL;
        writer() << "- OperatorPosition: " << node.GetFirstOpPosition() << EOL;

        if (node.GetOp() == BinaryOp::NotIn || node.GetOp() == BinaryOp::IsNot) {
            writer() << "- SecondaryOperatorPosition: " << node.GetSecondOpPosition() << EOL;
        }


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const UnaryExpression& node) {
    writer() << "<< UnaryExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- Operation: " << node.GetOp() << EOL;
        writer() << "- OperatorPosition: " << node.GetOpPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const DefinedExpression& node) {
    writer() << "<< DefinedExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- DefinedKeywordPosition: " << node.GetDefinedPosition() << EOL;
        writer() << "- ID: " << node.GetID() << EOL;

        if (node.GetInPosition().IsValid()) {
            writer() << "- InKeywordPosition: " << node.GetInPosition() << EOL;
        }

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const RaiseExpression& node) {
    writer() << "<< RaiseExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- RaiseKeywordPosition: " << node.GetRaisePosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const MemberAccessExpression& node) {
    writer() << "<< MemberAccessExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- MemberName: " << node.GetMemberName() << EOL;
        writer() << "- DotPosition: " << node.GetDotPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const SubscriptExpression& node) {
    writer() << "<< SubscriptExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- OpenSquarePosition: " << node.GetOpenSquarePosition() << EOL;
        writer() << "- CloseSquarePosition: " << node.GetCloseSquarePosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const InvocationExpression& node) {
    writer() << "<< InvocationExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- OpenParenPosition: " << node.GetOpenParenPosition() << EOL;
        writer() << "- CloseParenPosition: " << node.GetCloseParenPosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ParenthesizedExpression& node) {
    writer() << "<< ParenthesizedExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- OpenParenPosition: " << node.GetOpenParenPosition() << EOL;
        writer() << "- CloseParenPosition: " << node.GetCloseParenPosition() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const LiteralExpression& node) {
    writer() << "<< LiteralExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- LiteralType: " << node.GetLiteralType() << EOL;
        writer() << "- RawValue: " << m_source.GetString(node.GetRange()) << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ListExpression& node) {
    writer() << "<< ListExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- OpenSquarePosition: " << node.GetOpenSquarePosition() << EOL;
        writer() << "- CloseSquarePosition: " << node.GetCloseSquarePosition() << EOL;


    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const KeyValuePair& node) {
    writer() << "<< KeyValuePair @ " << node.GetRange() << " >>" << EOL << Indent;
    writer() << "- ColonPosition: " << node.GetColonPosition() << EOL;
    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const MapExpression& node) {
    writer() << "<< MapExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;
        writer() << "- OpenBracePosition: " << node.GetOpenBracePosition() << EOL;
        writer() << "- CloseBracePosition: " << node.GetCloseBracePosition() << EOL;

    return WalkerFlags::Continue;
}

WalkerFlags ASTDumper::OnEnterNode(const ClosureExpression& node) {
    writer() << "<< ClosureExpression @ " << node.GetRange() << " >>" << EOL;

    writer() << Indent;

    writer() << "- ArrowPosition: " << node.GetArrowPosition() << EOL;


    return WalkerFlags::Continue;
}

void ASTDumper::OnLeaveNode(const Parameters&) { writer() << Dedent; }
void ASTDumper::OnLeaveNode(const Label&) { writer() << Dedent; }
#define V(name, _) \
    void ASTDumper::OnLeaveNode(const name&) { writer() << Dedent; }

DECL_LIST(V)
STMT_LIST(V)
EXPR_LIST(V)
#undef V