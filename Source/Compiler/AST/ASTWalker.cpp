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
#include <BuildScript/Compiler/AST/ParameterList.h>
#include <BuildScript/Compiler/AST/Statements.h>

using namespace BuildScript;

void ASTWalker::Walk(ASTNode* root) {
    if (auto* decl = root->AsDeclaration()) {
        Walk(decl);
    }
    else if (auto* stmt = root->AsStatement()) {
        Walk(stmt);
    }
    else if (auto* expr = root->AsExpression()) {
        Walk(expr);
    }
    else if (auto* params = root->AsParameterList()) {
        Walk(params);
    }
    else if (auto* label = root->AsLabel()) {
        Walk(label);
    }
    else {
        NOT_REACHABLE;
    }
}

void ASTWalker::Walk(Declaration* node) {
    switch (node->GetKind()) {
    #define V(name, kind) \
        case DeclarationKind::kind: return Walk(node->As<name>());

        DECL_LIST(V)
    #undef V
        default:
            NOT_REACHABLE;
    }
}

void ASTWalker::Walk(Statement* node) {
    switch (node->GetKind()) {
    #define V(name, kind) \
        case StatementKind::kind: return Walk(node->As<name>());

            STMT_LIST(V)
    #undef V
        default:
            NOT_REACHABLE;
    }
}

void ASTWalker::Walk(Expression* node) {
    switch (node->GetKind()) {
    #define V(name, kind) \
        case ExpressionKind::kind: return Walk(node->As<name>());

            EXPR_LIST(V)
    #undef V
        default:
            NOT_REACHABLE;
    }
}

void ASTWalker::Walk(ParameterList* node) {
    // do nothing
}

void ASTWalker::Walk(Label* node) {
    if (!node->IsDefault()) { Walk(node->GetCaseValue()); }
}

void ASTWalker::Walk(InvalidDeclaration* node) {
    // do nothing
}

void ASTWalker::Walk(ScriptDeclaration* node) {
    for (auto* e : node->GetNodes()) {
        Walk(e);
    }
}

void ASTWalker::Walk(Parameter* node) {
    // do nothing
}

void ASTWalker::Walk(ImportDeclaration* node) {
    Walk(node->GetPath());
}

void ASTWalker::Walk(ExportDeclaration* node) {
    if (node->HasValue()) { Walk(node->GetValue()); }
}

void ASTWalker::Walk(FunctionDeclaration* node) {
    Walk(node->GetParameterList());
    Walk(node->GetBody());
}

void ASTWalker::Walk(ClassDeclaration* node) {
    for (auto* decl : node->GetMembers()) {
        Walk(decl);
    }
}

void ASTWalker::Walk(TaskDeclaration* node) {
    for (auto* decl : node->GetMembers()) {
        Walk(decl);
    }
}

void ASTWalker::Walk(VariableDeclaration* node) {
    Walk(node->GetValue());
}

void ASTWalker::Walk(TaskInputsDeclaration* node) {
    Walk(node->GetInputsValue());
    if (node->HasWith()) { Walk(node->GetWithValue()); }
}

void ASTWalker::Walk(TaskOutputsDeclaration* node) {
    Walk(node->GetOutputsValue());
    if (node->HasFrom()) { Walk(node->GetFromValue()); }
}

void ASTWalker::Walk(TaskActionDeclaration* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(TaskPropertyDeclaration* node) {
    Walk(node->GetValue());
}

void ASTWalker::Walk(ClassInitDeclaration* node) {
    Walk(node->GetParameterList());
    Walk(node->GetBody());
}

void ASTWalker::Walk(ClassDeinitDeclaration* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(ClassFieldDeclaration* node) {
    Walk(node->GetValue());
}

void ASTWalker::Walk(ClassMethodDeclaration* node) {
    Walk(node->GetParameterList());
    Walk(node->GetBody());
}

void ASTWalker::Walk(ClassPropertyDeclaration* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(InvalidStatement* node) {
    // do nothing
}

void ASTWalker::Walk(BlockStatement* node) {
    for (auto* e : node->GetNodes()) {
        Walk(e);
    }
}

void ASTWalker::Walk(ArrowStatement* node) {
    Walk(node->GetExpression());
}

void ASTWalker::Walk(IfStatement* node) {
    Walk(node->GetCondition());
    Walk(node->GetIfBody());
    if (node->HasElse()) { Walk(node->GetElseBody()); }
}

void ASTWalker::Walk(MatchStatement* node) {
    Walk(node->GetCondition());
    for (auto* stmt : node->GetStatements()) {
        // Shortcut: Statements in MatchStatement always LabeledStatement.
        auto* labeled = stmt->As<LabeledStatement>();
        NEVER_BE_NULL(labeled);

        Walk(labeled);
    }
}

void ASTWalker::Walk(LabeledStatement* node) {
    for (auto* label : node->GetLabels()) {
        Walk(label);
    }

    for (auto* e : node->GetNodes()) {
        Walk(e);
    }
}

void ASTWalker::Walk(ForStatement* node) {
    Walk(node->GetExpression());
    Walk(node->GetBody());
}

void ASTWalker::Walk(WhileStatement* node) {
    Walk(node->GetCondition());
    Walk(node->GetBody());
}

void ASTWalker::Walk(WithStatement* node) {
    Walk(node->GetExpression());
    Walk(node->GetBody());
}

void ASTWalker::Walk(TryStatement* node) {
    Walk(node->GetBody());
    for (auto* handler : node->GetHandlers()) {
        Walk(handler);
    }
}

void ASTWalker::Walk(ExceptStatement* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(FinallyStatement* node) {
    Walk(node->GetBody());
}

void ASTWalker::Walk(BreakStatement* node) {
    if (node->IsConditional()) { Walk(node->GetCondition()); }
}

void ASTWalker::Walk(ContinueStatement* node) {
    if (node->IsConditional()) { Walk(node->GetCondition()); }
}

void ASTWalker::Walk(ReturnStatement* node) {
    if (node->HasReturnValue()) { Walk(node->GetReturnValue()); }
}

void ASTWalker::Walk(AssertStatement* node) {
    Walk(node->GetCondition());
    if (node->HasMessage()) { Walk(node->GetMessage()); }
}

void ASTWalker::Walk(PassStatement* node) {
    // do nothing
}

void ASTWalker::Walk(AssignStatement* node) {
    Walk(node->GetTarget());
    Walk(node->GetValue());
}

void ASTWalker::Walk(InvalidExpression* node) {
    // do nothing
}

void ASTWalker::Walk(PassExpression* node) {
    // do nothing
}

void ASTWalker::Walk(TernaryExpression* node) {
    Walk(node->GetValueOnTrue());
    Walk(node->GetCondition());
    Walk(node->GetValueOnFalse());
}

void ASTWalker::Walk(BinaryExpression* node) {
    Walk(node->GetLeft());
    Walk(node->GetRight());
}

void ASTWalker::Walk(TypeTestExpression* node) {
    Walk(node->GetTarget());
}

void ASTWalker::Walk(ContainmentTestExpression* node) {
    Walk(node->GetValue());
    Walk(node->GetTarget());
}

void ASTWalker::Walk(UnaryExpression* node) {
    Walk(node->GetExpression());
}

void ASTWalker::Walk(DefinedExpression* node) {
    if (node->HasTarget()) { Walk(node->GetTarget()); }
}

void ASTWalker::Walk(RaiseExpression* node) {
    Walk(node->GetTarget());
}

void ASTWalker::Walk(MemberAccessExpression* node) {
    Walk(node->GetTarget());
}

void ASTWalker::Walk(InvocationExpression* node) {
    Walk(node->GetTarget());
    for (auto* arg : node->GetArguments()) {
        Walk(arg);
    }
}

void ASTWalker::Walk(SubscriptExpression* node) {
    Walk(node->GetTarget());
    Walk(node->GetIndex());
}

void ASTWalker::Walk(ParenthesizedExpression* node) {
    Walk(node->GetInnerExpression());
}

void ASTWalker::Walk(ListExpression* node) {
    for (auto* item : node->GetItems()) {
        Walk(item);
    }
}

void ASTWalker::Walk(MapExpression* node) {
    for (auto& [key, _, value] : node->GetItems()) {
        Walk(key);
        Walk(value);
    }
}

void ASTWalker::Walk(ClosureExpression* node) {
    Walk(node->GetParameterList());
    Walk(node->GetBody());
}

void ASTWalker::Walk(LiteralExpression* node) {
    if (node->GetLiteralType() == LiteralType::String) {
        for (auto* expr : node->GetInterpolations()) {
            Walk(expr);
        }
    }
}