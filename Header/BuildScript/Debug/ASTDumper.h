/*
 * ASTDumper.h
 * - Dump AST.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_NODEDUMPER_H
#define BUILDSCRIPT_COMPILER_AST_NODEDUMPER_H

#include <BuildScript/Compiler/AST/ASTVisitor.h>
#include <BuildScript/Debug/NodeWriter.h>

namespace BuildScript {
    /**
     * @brief Dump AST.
     */
    class ASTDumper : public ASTVisitor {
    private:
        NodeWriter& m_writer;

        void Visit(const ScriptNode*) override;
        void Visit(const Parameters*) override;
        void Visit(const TaskDeclaration*) override;
        void Visit(const ClassDeclaration*) override;
        void Visit(const ClassStaticMember*) override;
        void Visit(const ClassMethod*) override;
        void Visit(const ClassInit*) override;
        void Visit(const ClassDeinit*) override;
        void Visit(const ClassProperty*) override;
        void Visit(const ClassOperator*) override;
        void Visit(const FunctionDeclaration*) override;
        void Visit(const ImportDeclaration*) override;
        void Visit(const ExportDeclaration*) override;

        void Visit(const ErrorStatement*) override;
        void Visit(const BlockStatement*) override;
        void Visit(const AssignmentStatement*) override;
        void Visit(const IfStatement*) override;
        void Visit(const LabeledStatement*) override;
        void Visit(const MatchStatement*) override;
        void Visit(const ForStatement*) override;
        void Visit(const WhileStatement*) override;
        void Visit(const TryStatement*) override;
        void Visit(const RaiseStatement*) override;
        void Visit(const BreakStatement*) override;
        void Visit(const ContinueStatement*) override;
        void Visit(const ReturnStatement*) override;
        void Visit(const EmptyStatement*) override;
        void Visit(const AssertStatement*) override;
        void Visit(const VariableDeclaration*) override;

        void Visit(const ErrorExpression*) override;
        void Visit(const TernaryExpression*) override;
        void Visit(const BinaryExpression*) override;
        void Visit(const CastExpression*) override;
        void Visit(const UnaryExpression*) override;
        void Visit(const DefinedExpression*) override;
        void Visit(const MemberAccessExpression*) override;
        void Visit(const SubscriptExpression*) override;
        void Visit(const InvocationExpression*) override;
        void Visit(const LiteralExpression*) override;
        void Visit(const StringLiteral*) override;
        void Visit(const MapExpression*) override;
        void Visit(const ListExpression*) override;
        void Visit(const TupleExpression*) override;
        void Visit(const NamedTupleExpression*) override;
        void Visit(const ClosureExpression*) override;

        inline void SafeAccept(const ASTNode* node) {
            if (node == nullptr) {
                m_writer << "<nullptr>" << EOL;
            } else {
                node->Accept(this);
            }
        }

        inline void SafeAccept(const Parameters* node) {
            if (node == nullptr) {
                m_writer << "<nullptr>" << EOL;
            } else {
                node->Accept(this);
            }
        }

    public:
        /**
         * @brief Construct dumper.
         * @param writer the writer.
         * @see BuildScript::NodeWriter
         */
        ASTDumper(NodeWriter& writer)
            : m_writer(writer) {}

        /**
         * @brief Dump nodes.
         * @param node the ScriptNode node.
         */
        void Dump(ScriptNode* node) { Visit(node); }
    }; // end class ASTDumper
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_NODEDUMPER_H