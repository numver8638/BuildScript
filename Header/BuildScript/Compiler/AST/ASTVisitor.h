/*
 * ASTVisitor.h
 * - Base class for Visit pattern in AST.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_ASTVISITOR_H
#define BUILDSCRIPT_COMPILER_AST_ASTVISITOR_H

#include <BuildScript/Compiler/AST/ScriptNode.h>
#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/AST/Expressions.h>

namespace BuildScript {
    /**
     * @brief Base class for Visit pattern in AST.
     */
    class ASTVisitor {
    public:
        virtual ~ASTVisitor() = default;

        // Script
        virtual void Visit(const ScriptNode*) = 0;
        
        // Declarations
        virtual void Visit(const Parameters*) = 0;
        virtual void Visit(const TaskDeclaration*) = 0;
        virtual void Visit(const ClassDeclaration*) = 0;
        virtual void Visit(const FunctionDeclaration*) = 0;
        virtual void Visit(const ImportDeclaration*) = 0;
        virtual void Visit(const ExportDeclaration*) = 0;

        // Class members
        virtual void Visit(const ClassStaticMember*) = 0;
        virtual void Visit(const ClassMethod*) = 0;
        virtual void Visit(const ClassInit*) = 0;
        virtual void Visit(const ClassDeinit*) = 0;
        virtual void Visit(const ClassProperty*) = 0;
        virtual void Visit(const ClassOperator*) = 0;

        // Statements
        virtual void Visit(const ErrorStatement*) = 0;
        virtual void Visit(const BlockStatement*) = 0;
        virtual void Visit(const AssignmentStatement*) = 0;
        virtual void Visit(const IfStatement*) = 0;
        virtual void Visit(const LabeledStatement*) = 0;
        virtual void Visit(const MatchStatement*) = 0;
        virtual void Visit(const ForStatement*) = 0;
        virtual void Visit(const WhileStatement*) = 0;
        virtual void Visit(const TryStatement*) = 0;
        virtual void Visit(const RaiseStatement*) = 0;
        virtual void Visit(const BreakStatement*) = 0;
        virtual void Visit(const ContinueStatement*) = 0;
        virtual void Visit(const ReturnStatement*) = 0;
        virtual void Visit(const EmptyStatement*) = 0;
        virtual void Visit(const AssertStatement*) = 0;
        virtual void Visit(const VariableDeclaration*) = 0;

        // Expressions
        virtual void Visit(const ErrorExpression*) = 0;
        virtual void Visit(const TernaryExpression*) = 0;
        virtual void Visit(const BinaryExpression*) = 0;
        virtual void Visit(const CastExpression*) = 0;
        virtual void Visit(const UnaryExpression*) = 0;
        virtual void Visit(const DefinedExpression*) = 0;
        virtual void Visit(const MemberAccessExpression*) = 0;
        virtual void Visit(const SubscriptExpression*) = 0;
        virtual void Visit(const InvocationExpression*) = 0;
        virtual void Visit(const LiteralExpression*) = 0;
        virtual void Visit(const StringLiteral*) = 0;
        virtual void Visit(const MapExpression*) = 0;
        virtual void Visit(const ListExpression*) = 0;
        virtual void Visit(const TupleExpression*) = 0;
        virtual void Visit(const NamedTupleExpression*) = 0;
        virtual void Visit(const ClosureExpression*) = 0;
    }; // end class ASTVisitor
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ASTVISITOR_H