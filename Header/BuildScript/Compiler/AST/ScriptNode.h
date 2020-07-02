/*
 * ScriptNode.h
 * - Represent parsed script.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_SCRIPTNODE_H
#define BUILDSCRIPT_COMPILER_AST_SCRIPTNODE_H

#include <string>
#include <utility>
#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>

namespace BuildScript {
    class ASTDumper;

    /**
     * @brief Represent parsed script.
     */
    class ScriptNode {
        std::string m_filename;
        std::vector<ASTNode*> m_nodes;

    public:
        explicit ScriptNode(std::string filename)
            : m_filename(std::move(filename)) {}

        ~ScriptNode() {
            for (auto e : m_nodes)
                delete e;
        }

        const std::string& GetFileName() const { return m_filename; }

        const std::vector<ASTNode*>& GetNodes() const { return m_nodes; }

        void Append(ASTNode* node) {
            m_nodes.push_back(node);
        }
    }; // end class ScriptNode
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_SCRIPTNODE_H