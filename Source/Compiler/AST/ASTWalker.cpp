/*
 * ASTWalker.cpp
 * - .
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

void ASTWalker::Enter(const ASTNode* node) {
    if (auto* param = node->AsParameters()) {
        OnEnterNode(*param);
    }
    else if (auto* label = node->AsLabel()) {
        OnEnterNode(*label);
    }
    else if (auto* decl = node->AsDeclaration()) {
        #define V(name, kind) \
            case DeclarationKind::kind: \
                OnEnterNode(*(decl->As<name>())); \
                break;

        switch (decl->GetKind()) {
            DECL_LIST(V) // NOLINT

            default:
                NOT_REACHABLE;
        }

        #undef V
    }
    else if (auto* stmt = node->AsStatement()) {
        #define V(name, kind) \
            case StatementKind::kind: \
                OnEnterNode(*(stmt->As<name>())); \
                break;

        switch (stmt->GetKind()) {
            STMT_LIST(V) // NOLINT

            default:
                NOT_REACHABLE;
        }

        #undef V
    }
    else if (auto* expr = node->AsExpression()) {
        #define V(name, kind) \
            case ExpressionKind::kind: \
                OnEnterNode(*(expr->As<name>())); \
                break;

        switch (expr->GetKind()) {
            EXPR_LIST(V) // NOLINT

            default:
                NOT_REACHABLE;
        }

        #undef V
    }
    else {
        NOT_REACHABLE;
    }

}

void ASTWalker::Leave(const ASTNode* node) {
    if (auto* param = node->AsParameters()) {
        OnLeaveNode(*param);
    }
    else if (auto* label = node->AsLabel()) {
        OnLeaveNode(*label);
    }
    else if (auto* decl = node->AsDeclaration()) {
        #define V(name, kind) \
            case DeclarationKind::kind: \
                OnLeaveNode(*(decl->As<name>())); \
                break;

        switch (decl->GetKind()) {
            DECL_LIST(V)

            default:
                NOT_REACHABLE;
        }

        #undef V
    }
    else if (auto* stmt = node->AsStatement()) {
        #define V(name, kind) \
            case StatementKind::kind: \
                OnLeaveNode(*(stmt->As<name>())); \
                break;

        switch (stmt->GetKind()) {
            STMT_LIST(V)

            default:
                NOT_REACHABLE;
        }

        #undef V
    }
    else if (auto* expr = node->AsExpression()) {
        #define V(name, kind) \
            case ExpressionKind::kind: \
                OnLeaveNode(*(expr->As<name>())); \
                break;

        switch (expr->GetKind()) {
            EXPR_LIST(V)

            default:
                NOT_REACHABLE;
        }

        #undef V
    }
    else {
        NOT_REACHABLE;
    }
}

void ASTWalker::Walk(const ASTNode* root) {
    Enter(root);

    for (const auto* node : root->GetChildren()) {
        Walk(node);
    }

    Leave(root);
}