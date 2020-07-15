/*
 * ASTDumper.Expression.cpp
 * - Dump AST.
 *   This file is part of ASTDumper and contains code printing expressions.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Debug/ASTDumper.h>

#include <BuildScript/Debug/NodeWriter.h>

using namespace BuildScript;

void ASTDumper::Visit(const ErrorExpression* node) {
    m_writer << "+ ErrorExpression @ " << node->GetPosition() << EOL;
}

void ASTDumper::Visit(const TernaryExpression* node) {
    m_writer << "+ TernaryExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Condition:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetCondition());
        }
        m_writer << "- ValueOnTrue:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetValueOnTrue());
        }
        m_writer << "- ValueOnFalse:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetValueOnFalse());
        }
    }
}

void ASTDumper::Visit(const BinaryExpression* node) {
    static const char* const strtab[] = {
        "*",
        "/",
        "%",
        "+",
        "-",
        "<<",
        ">>",
        "&",
        "^",
        "|",
        "<",
        "<=",
        ">",
        ">=",
        "in",
        "not in",
        "==",
        "!=",
        "is",
        "is not",
        "and",
        "or"
    };

    m_writer << "+ BinaryExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Operator: " << strtab[static_cast<size_t>(node->GetOperator())] << EOL;
        m_writer << "- Left:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetLeft());
        }
        m_writer << "- Right:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetRight());
        }
    }
}

void ASTDumper::Visit(const CastExpression* node) {
    m_writer << "+ CastExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Typename: " << node->GetTypeName() << EOL;
        m_writer << "- Target:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetTarget());
        }
    }
}

void ASTDumper::Visit(const UnaryExpression* node) {
    static const char* const strtab[] = {
        "+",
        "-",
        "~",
        "not"
    };

    m_writer << "+ UnaryExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Operator: " << strtab[static_cast<size_t>(node->GetOperator())] << EOL;
        m_writer << "- Expression:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetExpression());
        }
    }
}

void ASTDumper::Visit(const DefinedExpression* node) {
    m_writer << "+ DefinedExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- ID: " << node->GetID() << EOL;
        if (node->GetTarget() != nullptr) {
            m_writer << "- Target" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(node->GetTarget());
            }
        }
    }
}

void ASTDumper::Visit(const MemberAccessExpression* node) {
    m_writer << "+ MemberAccessExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Member: " << node->GetMemberName() << EOL;
        m_writer << "- Target" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetTarget());
        }
    }
}

void ASTDumper::Visit(const SubscriptExpression* node) {
    m_writer << "+ SubscriptExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Index:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetIndex());
        }
        m_writer << "- Target" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetTarget());
        }
    }
}

void ASTDumper::Visit(const InvocationExpression* node) {
    m_writer << "+ InvocationExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Arguments:" << EOL;
        {
            IndentRAII indent2{m_writer};
            for (auto e : node->GetArguments())
                SafeAccept(e);
        }
        m_writer << "- Target" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetTarget());
        }
    }
}

void ASTDumper::Visit(const LiteralExpression* node) {
    static const char* const strtab[] = {
        "Identifier",
        "Integer",
        "Float",
        "String",
        "True",
        "False",
        "None",
        "Self",
        "Super"
    };

    m_writer << "+ LiteralExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Type: " << strtab[static_cast<size_t>(node->GetLiteralKind())] << EOL;
        m_writer << "- Image: " << node->GetImage() << EOL;
    }
}

void ASTDumper::Visit(const StringLiteral* node) {
    m_writer << "+ StringLiteral @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- RawString: " << node->GetImage() << EOL;
        m_writer << "- String: " << node->GetText() << EOL;
        {
            IndentRAII indent2{m_writer};
            m_writer << "- InterpolatedExpressions:" << EOL;
            {
                IndentRAII indent3{m_writer};
                for (auto e : node->GetInterpolatedExpressions())
                    SafeAccept(e);
            }
        }
    }
}

void ASTDumper::Visit(const MapExpression* node) {
    m_writer << "+ MapExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Items:" << EOL;
        for (auto& e : node->GetItems()) {
            m_writer << "- Key:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(e.first);
            }
            m_writer << "- Value:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(e.second);
            }
        }
    }
}

void ASTDumper::Visit(const ListExpression* node) {
    m_writer << "+ ListExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Items:" << EOL;
        for (auto e : node->GetItems()) {
            IndentRAII indent2{m_writer};
            SafeAccept(e);
        }
    }
}

void ASTDumper::Visit(const TupleExpression* node) {
    m_writer << "+ TupleExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Items:" << EOL;
        for (auto e : node->GetItems()) {
            IndentRAII indent2{m_writer};
            SafeAccept(e);
        }
    }
}

void ASTDumper::Visit(const NamedTupleExpression* node) {
    m_writer << "+ NamedTupleExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        for (auto& e : node->GetItems()) {
            m_writer << "- Key:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(e.first);
            }
            m_writer << "- Value:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(e.second);
            }
        }
    }
}

void ASTDumper::Visit(const ClosureExpression* node) {
    m_writer << "+ ClosureExpression @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Parameters:" << EOL;
        {
            IndentRAII indent2{m_writer};
            if (node->GetParameters().empty()) {
                m_writer << "- <empty>" << EOL;
            } else {
                for (auto& e : node->GetParameters()) {
                    m_writer << "- " << e.first << "@" << e.second << EOL;
                }
            }
        }
        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetBody());
        }
    }
}
