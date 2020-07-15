/*
 * ASTDumper.cpp
 * - Dump AST.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Debug/ASTDumper.h>

#include <BuildScript/Debug/NodeWriter.h>

using namespace BuildScript;

void ASTDumper::Visit(const ScriptNode* node) {
    m_writer << "+ ScriptNode @ " << node->GetFileName() << EOL;
    {
        IndentRAII indent{m_writer};
        for (auto e : node->GetNodes())
            SafeAccept(e);
    }
    m_writer << "\n";
}