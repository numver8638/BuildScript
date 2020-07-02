/*
 * ASTDumper.Statement.cpp
 * - Dump AST.
 *   This file is part of ASTDumper and contains code printing statements.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Debug/ASTDumper.h>

#include <BuildScript/Debug/NodeWriter.h>

using namespace BuildScript;

void ASTDumper::Visit(const ErrorStatement* node) {
    m_writer << "+ ErrorStatement @ " << node->GetPosition() << EOL;
}

void ASTDumper::Visit(const BlockStatement* node) {
    IndentRAII indent{m_writer};
    for (auto e : node->GetStatements())
        SafeAccept(e);
}

void ASTDumper::Visit(const AssignmentStatement* node) {
    static const char* const strtab[] = {
        "=",
        "+=",
        "-=",
        "*=",
        "/=",
        "%=",
        "&=",
        "|=",
        "^=",
        "<<=",
        ">>="
    };

    m_writer << "+ AssignmentStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        m_writer << "- Operator: " << strtab[static_cast<size_t>(node->GetOperator())] << EOL;

        m_writer << "- Target:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetTarget());
        }

        m_writer << "- Value:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetValue());
        }
    }
}

void ASTDumper::Visit(const IfStatement* node) {
    m_writer << "+ IfStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        m_writer << "- ThenClauses:" << EOL;
        {
            IndentRAII indent2{m_writer};
            for (auto& e : node->GetThenClauses()) {
                m_writer << "- Condition:" << EOL;
                {
                    IndentRAII indent3{m_writer};
                    SafeAccept(e.first);
                }
                m_writer << "- Body:" << EOL;
                {
                    IndentRAII indent3{m_writer};
                    SafeAccept(e.second);
                }
            }
        }

        m_writer << "- ElseClause:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetElseClause());
        }
    }
}

void ASTDumper::Visit(const LabeledStatement* node) {
    m_writer << "+ LabeledStatement" << EOL;
    {
        IndentRAII indent{m_writer};

        m_writer << "- Labels:" << EOL;
        {
            for (auto& e : node->GetLabels()) {
                SourcePosition pos;
                Expression* expr;

                std::tie(pos, expr) = e;

                m_writer << "- " << (expr == nullptr ? "Default" : "Case") << " @ " << pos << EOL;

                if (expr != nullptr) {
                    IndentRAII indent2{m_writer};
                    m_writer << "- Condition:" << EOL;
                    {
                        IndentRAII indent3{m_writer};
                        SafeAccept(std::get<1>(e));
                    }
                }
            }
        }

        m_writer << "- Statements:" << EOL;
        {
            IndentRAII indent2{m_writer};
            for (auto e : node->GetStatements())
                SafeAccept(e);
        }
    }
}

void ASTDumper::Visit(const MatchStatement* node) {
    m_writer << "+ MatchStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        m_writer << "- Target:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetTarget());
        }

        m_writer << "- Statements:" << EOL;
        {
            IndentRAII indent2{m_writer};
            for (auto e : node->GetLabeledStatements()) {
                SafeAccept(e);
            }
        }
    }
}

void ASTDumper::Visit(const ForStatement* node) {
    m_writer << "+ ForStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        m_writer << "- Parameters:" << EOL;
        {
            IndentRAII indent2{m_writer};
            for (auto& e : node->GetParameters()) {
                m_writer << e << EOL;
            }
        }

        m_writer << "- Target:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetExpression());
        }

        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetBody());
        }
    }
}

void ASTDumper::Visit(const WhileStatement* node) {
    m_writer << "+ WhileStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        m_writer << "- Condition:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetCondition());
        }

        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetBody());
        }
    }
}

void ASTDumper::Visit(const TryStatement* node) {
    m_writer << "+ TryStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetBody());
        }

        m_writer << "- ExceptClauses:" << EOL;
        {
            IndentRAII indent2{m_writer};
            if (node->GetExceptClauses().empty()) {
                m_writer << "<empty>" << EOL;
            } else {
                for (auto& e : node->GetExceptClauses()) {
                    m_writer << "+ ExceptClause @ " << e->GetPosition() << EOL;
                    {
                        IndentRAII indent3{m_writer};
                        m_writer << "- TypeName: " << e->GetTypeName() << EOL;
                        m_writer << "- VarName: " << e->GetVarName() << EOL;
                        m_writer << "- Body:" << EOL;
                        {
                            IndentRAII indent4{m_writer};
                            SafeAccept(e->GetBody());
                        }
                    }
                }
            }
        }

        m_writer << "- Finally:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetFinallyClause());
        }
    }
}

void ASTDumper::Visit(const RaiseStatement* node) {
    m_writer << "+ RaiseStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Expression:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetExpression());
        }
    }
}

void ASTDumper::Visit(const BreakStatement* node) {
    m_writer << "+ BreakStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        if (node->GetCondition() != nullptr) {
            m_writer << "- Condition:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(node->GetCondition());
            }
        }
    }
}

void ASTDumper::Visit(const ContinueStatement* node) {
    m_writer << "+ ContinueStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};

        if (node->GetCondition() != nullptr) {
            m_writer << "- Condition:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(node->GetCondition());
            }
        }
    }
}

void ASTDumper::Visit(const ReturnStatement* node) {
    m_writer << "+ ReturnStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        if (node->GetExpression() != nullptr) {
            m_writer << "- ReturnValue:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(node->GetExpression());
            }
        }
    }
}

void ASTDumper::Visit(const EmptyStatement* node) {
    m_writer << "+ EmptyStatement @ " << node->GetPosition() << EOL;
}

void ASTDumper::Visit(const AssertStatement* node) {
    m_writer << "+ AssertStatement @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Condition:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetCondition());
        }
        if (node->GetMessage() != nullptr) {
            m_writer << "- Message:" << EOL;
            {
                IndentRAII indent3{m_writer};
                SafeAccept(node->GetMessage());
            }
        }
    }
}

void ASTDumper::Visit(const VariableDeclaration* node) {
    m_writer << "+ VariableDeclaration @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Name: " << node->GetName() << EOL;
        if (node->GetValue() != nullptr) {
            m_writer << "- Value:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(node->GetValue());
            }
        }
    }
}