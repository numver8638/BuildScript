/*
 * ASTDumper.Declaration.cpp
 * - Dump AST.
 *   This file is part of ASTDumper and contains code printing declarations.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Debug/ASTDumper.h>

#include <BuildScript/Debug/NodeWriter.h>

using namespace BuildScript;

void ASTDumper::Visit(const Parameters* node) {
    IndentRAII indent{m_writer};

    if (node->GetParameters().empty() && !node->IsVaradic()) {
        m_writer << "- <empty>" << EOL;
    } else {
        for (auto& e : node->GetParameters()) {
            m_writer << "- " << e.first << " @ " << e.second << EOL;
        }
    }

    if (node->IsVaradic()) {
        auto e = node->GetVaradic();
        m_writer << "- VaradicParameter: " << e.first << " @ " << e.second << EOL;
    }

}

void ASTDumper::Visit(const TaskDeclaration* node) {
    m_writer << "+ TaskDeclaration @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Name: " << node->GetName() << EOL;
        m_writer << "- Extends: " << node->GetExtends() << EOL;
        m_writer << "- Dependencies: ";
        for (auto& e : node->GetDependencies()) {
            m_writer << e << ", ";
        }
        m_writer << EOL;
        m_writer << "- Inputs @ " << node->GetInputsClause().Position << EOL;
        {
            IndentRAII indent2{m_writer};
            m_writer << "- Value:" << EOL;
            {
                IndentRAII indent3{m_writer};
                SafeAccept(node->GetInputsClause().Value);
            }
            m_writer << "- With:" << EOL;
            {
                IndentRAII indent3{m_writer};
                SafeAccept(node->GetInputsClause().With);
            }
        }
        m_writer << "- Outputs @ " << node->GetOuputsClause().Position << EOL;
        {
            IndentRAII indent2{m_writer};
            m_writer << "- Value:" << EOL;
            {
                IndentRAII indent3{m_writer};
                SafeAccept(node->GetOuputsClause().Value);
            }
            m_writer << "- From:" << EOL;
            {
                IndentRAII indent3{m_writer};
                SafeAccept(node->GetOuputsClause().From);
            }
        }
        m_writer << "- Properties:" << EOL;
        {
            IndentRAII indent2{m_writer};
            for (auto& e : node->GetProperties()) {
                m_writer << "- Position: " << e.Position << EOL;
                m_writer << "- Name: " << e.Name << EOL;
                m_writer << "- Value:" << EOL;
                {
                    IndentRAII indent3{m_writer};
                    SafeAccept(e.Value);
                }
            }
        }
        m_writer << "- Action:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetDoBlock());
        }
        m_writer << "- BeforeAction:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetDoFirstBlock());
        }
        m_writer << "- AfterAction:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetDoLastBlock());
        }
    }
}

void ASTDumper::Visit(const ClassDeclaration* node) {
    m_writer << "+ ClassDeclaration @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Name: " << node->GetName() << EOL;
        m_writer << "- Extends: " << node->GetExtends() << EOL;
        m_writer << "- Members:" << EOL;
        {
            IndentRAII indent2{m_writer};
            for (auto e : node->GetMembers()) {
                SafeAccept(e);
            }
        }
    }
}

void ASTDumper::Visit(const FunctionDeclaration* node) {
    m_writer << "+ FunctionDeclaration @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        m_writer << "- Name: " << node->GetName() << EOL;
        m_writer << "- Parameters" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetParameters());
        }
        m_writer << "- Body:" << EOL;
        {
            IndentRAII indent2{m_writer};
            SafeAccept(node->GetBody());
        }
    }
}

void ASTDumper::Visit(const ImportDeclaration* node) {
    m_writer << "+ ImportDeclaration @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        for (auto& e : node->GetImports()) {
            m_writer << "- Value:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(e);
            }
        }
    }
}

void ASTDumper::Visit(const ExportDeclaration* node) {
    m_writer << "+ ExportDeclaration @ " << node->GetPosition() << EOL;
    {
        IndentRAII indent{m_writer};
        for (auto& e : node->GetExports()) {
            m_writer << "- Name: " << e.Name << EOL;
            m_writer << "- Position: " << e.Position << EOL;
            m_writer << "- Value:" << EOL;
            {
                IndentRAII indent2{m_writer};
                SafeAccept(e.Value);
            }
        }
    }
}
