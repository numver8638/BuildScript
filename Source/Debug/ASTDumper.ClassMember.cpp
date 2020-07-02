/*
 * ASTDumper.ClassMember.cpp
 * - Dump AST.
 *   This file is part of ASTDumper and contains code printing class members.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Debug/ASTDumper.h>

#include <BuildScript/Debug/NodeWriter.h>

using namespace BuildScript;

void ASTDumper::Visit(const ClassStaticMember* node) {
    m_writer << "+ ClassStaticMember @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Name: " << node->GetName() << EOL;
        m_writer << "- Value:" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetValue()->Accept(this);
        }
    }
}

void ASTDumper::Visit(const ClassMethod* node) {
    m_writer << "+ ClassMethod @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Name: " << node->GetName() << EOL;
        m_writer << "- Static: " << node->IsStaticMethod() << EOL;
        m_writer << "- Parameters" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetParameters()->Accept(this);
        }
        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetBody()->Accept(this);
        }
    }
}

void ASTDumper::Visit(const ClassInit* node) {
    m_writer << "+ ClassInit @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Parameters" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetParameters()->Accept(this);
        }
        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetBody()->Accept(this);
        }
    }
}

void ASTDumper::Visit(const ClassDeinit* node) {
    m_writer << "+ ClassDeinit @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetBody()->Accept(this);
        }
    }
}

void ASTDumper::Visit(const ClassProperty* node) {
    m_writer << "+ ClassProperty @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Name: " << node->GetName() << EOL;
        m_writer << "- Type:" << (node->GetType() == PropertyType::Getter ? "Getter" : "Setter") << EOL;
        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetBody()->Accept(this);
        }
    }
}

void ASTDumper::Visit(const ClassOperator* node) {
    static const char* const strtab[] = {
        "<error>",
        "+", "+=", "-", "-=", "*", "*=", "/", "/=", "%",
        "%=", "&", "&=", "|", "|=", "^", "^=", "<", "<=",
        "<<", ">", ">=", ">>", "==", "!=", "[]"
    };

    m_writer << "+ ClassOperator @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Operator: " << strtab[static_cast<size_t>(node->GetOperator())] << EOL;
        m_writer << "- Parameters" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetParameters()->Accept(this);
        }
        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            node->GetBody()->Accept(this);
        }
    }
}