/*
 * ASTWalker.cpp
 * - Iterate nodes in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/ASTWalker.h>

#include <BuildScript/Assert.h>
#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/AST/Parameters.h>
#include <BuildScript/Compiler/AST/Statements.h>

using namespace BuildScript;

void ASTWalker::Walk(const ASTNode* root) {
    if (auto* decl = root->AsDeclaration()) {
        Walk(decl);
    }
    else if (auto* stmt = root->AsStatement()) {
        Walk(stmt);
    }
    else if (auto* expr = root->AsExpression()) {
        Walk(expr);
    }
    else if (auto* params = root->AsParameters()) {
        Walk(params);
    }
    else if (auto* label = root->AsLabel()) {
        Walk(label);
    }
    else {
        NOT_REACHABLE;
    }
}

void ASTWalker::Walk(const Declaration* node) {
    switch (node->GetKind()) {
    #define V(name, kind) \
        case DeclarationKind::kind: return Walk(node->As<name>());

        DECL_LIST(V)
    #undef V
        default:
            NOT_REACHABLE;
    }
}

void ASTWalker::Walk(const Statement* node) {
    switch (node->GetKind()) {
    #define V(name, kind) \
        case StatementKind::kind: return Walk(node->As<name>());

            STMT_LIST(V)
    #undef V
        default:
            NOT_REACHABLE;
    }
}

void ASTWalker::Walk(const Expression* node) {
    switch (node->GetKind()) {
    #define V(name, kind) \
        case ExpressionKind::kind: return Walk(node->As<name>());

            EXPR_LIST(V)
    #undef V
        default:
            NOT_REACHABLE;
    }
}

void ASTWalker::Walk(const Parameters* node) {
    // do nothing
}

void ASTWalker::Walk(const Label* node) {
    if (!node->IsDefault()) { Walk(node->GetCaseValue()); }
}

void ASTWalker::Walk(const InvalidDeclaration* node) {
    // do nothing
}

void ASTWalker::Walk(const ScriptDeclaration* node) {
    for (const auto* e : node->GetNodes()) {
        Walk(e);
    }
}

void ASTWalker::Walk(const ImportDeclaration* node) {
    Walk(node->GetPath());
}

void ASTWalker::Walk(const ExportDeclaration* node) {
    if (node->HasValue()) { Walk(node->GetValue()); }
}

void ASTWalker::Walk(const FunctionDeclaration* node) {
    Walk(node->GetParameters());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const ClassDeclaration* node) {
    for (const auto* decl : node->GetMembers()) {
        Walk(decl);
    }
}

void ASTWalker::Walk(const TaskDeclaration* node) {
    for (const auto* decl : node->GetMembers()) {
        Walk(decl);
    }
}

void ASTWalker::Walk(const VariableDeclaration* node) {
    Walk(node->GetValue());
}

void ASTWalker::Walk(const TaskInputsDeclaration* node) {
    Walk(node->GetInputsValue());
    if (node->HasWith()) { Walk(node->GetWithValue()); }
}

void ASTWalker::Walk(const TaskOutputsDeclaration* node) {
    Walk(node->GetOutputsValue());
    if (node->HasFrom()) { Walk(node->GetFromValue()); }
}

void ASTWalker::Walk(const TaskActionDeclaration* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(const TaskPropertyDeclaration* node) {
    Walk(node->GetValue());
}

void ASTWalker::Walk(const ClassInitDeclaration* node) {
    Walk(node->GetParameters());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const ClassDeinitDeclaration* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(const ClassFieldDeclaration* node) {
    Walk(node->GetValue());
}

void ASTWalker::Walk(const ClassMethodDeclaration* node) {
    Walk(node->GetParameters());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const ClassPropertyDeclaration* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(const ClassOperatorDeclaration* node) {
    Walk(node->GetParameters());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const InvalidStatement* node) {
    // do nothing
}

void ASTWalker::Walk(const BlockStatement* node) {
    for (const auto* e : node->GetNodes()) {
        Walk(e);
    }
}

void ASTWalker::Walk(const ArrowStatement* node) {
    Walk(node->GetExpression());
}

void ASTWalker::Walk(const IfStatement* node) {
    Walk(node->GetCondition());
    Walk(node->GetIfBody());
    if (node->HasElse()) { Walk(node->GetElseBody()); }
}

void ASTWalker::Walk(const MatchStatement* node) {
    Walk(node->GetCondition());
    for (const auto* stmt : node->GetStatements()) {
        // Shortcut: Statements in MatchStatement always LabeledStatement.
        const auto* labeled = stmt->As<LabeledStatement>();
        NEVER_BE_NULL(labeled);

        Walk(labeled);
    }
}

void ASTWalker::Walk(const LabeledStatement* node) {
    for (const auto* label : node->GetLabels()) {
        Walk(label);
    }

    for (const auto* e : node->GetNodes()) {
        Walk(e);
    }
}

void ASTWalker::Walk(const ForStatement* node) {
    Walk(node->GetExpression());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const WhileStatement* node) {
    Walk(node->GetCondition());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const WithStatement* node) {
    Walk(node->GetExpression());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const TryStatement* node) {
    Walk(node->GetBody());
    for (const auto* handler : node->GetHandlers()) {
        Walk(handler);
    }
}

void ASTWalker::Walk(const ExceptStatement* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(const FinallyStatement* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(const BreakStatement* node) {
    if (node->IsConditional()) { Walk(node->GetCondition()); }
}

void ASTWalker::Walk(const ContinueStatement* node) {
    if (node->IsConditional()) { Walk(node->GetCondition()); }
}

void ASTWalker::Walk(const ReturnStatement* node) {
    if (node->HasReturnValue()) { Walk(node->GetReturnValue()); }
}

void ASTWalker::Walk(const AssertStatement* node) {
    Walk(node->GetCondition());
    if (node->HasMessage()) { Walk(node->GetMessage()); }
}

void ASTWalker::Walk(const PassStatement* node) {
    // do nothing
}

void ASTWalker::Walk(const AssignStatement* node) {
    Walk(node->GetTarget());
    Walk(node->GetValue());
}

void ASTWalker::Walk(const InvalidExpression* node) {
    // do nothing
}

void ASTWalker::Walk(const PassExpression* node) {
    // do nothing
}

void ASTWalker::Walk(const KeyValuePair* node) {
    Walk(node->GetKey());
    Walk(node->GetValue());
}

void ASTWalker::Walk(const TernaryExpression* node) {
    Walk(node->GetValueOnTrue());
    Walk(node->GetCondition());
    Walk(node->GetValueOnFalse());
}

void ASTWalker::Walk(const BinaryExpression* node) {
    Walk(node->GetLeft());
    Walk(node->GetRight());
}

void ASTWalker::Walk(const TypeTestExpression* node) {
    Walk(node->GetTarget());
}

void ASTWalker::Walk(const ContainmentTestExpression* node) {
    Walk(node->GetValue());
    Walk(node->GetTarget());
}

void ASTWalker::Walk(const UnaryExpression* node) {
    Walk(node->GetExpression());
}

void ASTWalker::Walk(const DefinedExpression* node) {
    if (node->HasTarget()) { Walk(node->GetTarget()); }
}

void ASTWalker::Walk(const RaiseExpression* node) {
    Walk(node->GetTarget());
}

void ASTWalker::Walk(const MemberAccessExpression* node) {
    Walk(node->GetTarget());
}

void ASTWalker::Walk(const InvocationExpression* node) {
    Walk(node->GetTarget());
    for (const auto* arg : node->GetArguments()) {
        Walk(arg);
    }
}

void ASTWalker::Walk(const SubscriptExpression* node) {
    Walk(node->GetTarget());
    Walk(node->GetIndex());
}

void ASTWalker::Walk(const ParenthesizedExpression* node) {
    Walk(node->GetInnerExpression());
}

void ASTWalker::Walk(const ListExpression* node) {
    for (const auto* item : node->GetItems()) {
        Walk(item);
    }
}

void ASTWalker::Walk(const MapExpression* node) {
    for (auto& [key, _, value] : node->GetItems()) {
        Walk(key);
        Walk(value);
    }
}

void ASTWalker::Walk(const ClosureExpression* node) {
    Walk(node->GetParameters());
    Walk(node->GetBody());
}

void ASTWalker::Walk(const LiteralExpression* node) {
    if (node->GetLiteralType() == LiteralType::String) {
        for (const auto* expr : node->GetInterpolations()) {
            Walk(expr);
        }
    }
}