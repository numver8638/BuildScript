/*
 * IRGenerator.Statement.cpp
 * - Intermediate Representation code generator.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRGenerator.h>

#include <algorithm>

#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/IR/Statements.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>

using namespace BuildScript;

void IRGenerator::Walk(InvalidStatement* node) {
    NEVER_BE_CALLED("cannot be generated with erroneous script.");
}

void IRGenerator::Walk(BlockStatement* node) {
    super::Walk(node);
}

void IRGenerator::Walk(ArrowStatement* node) {
    super::Walk(node);
}

void IRGenerator::Walk(IfStatement* node) {
    auto cond = EvaluateExpr(node->GetCondition());
    BasicBlock* thenBB = GetBuilder().CreateNewBlock();
    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();
    BasicBlock* elseBB = node->HasElse() ? GetBuilder().CreateNewBlock() : mergeBB;

    GetBuilder().InsertBack<IRConditionalBranchOp>(node->GetIfPosition(), cond, thenBB, elseBB);
    GetBuilder().GetCurrentBlock().AddSuccessors({ thenBB, elseBB });

    // then
    GetBuilder().SetBlock(thenBB);
    super::Walk(node->GetIfBody());
    GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
    GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);

    // else
    if (node->HasElse()) {
        GetBuilder().SetBlock(elseBB);
        super::Walk(node->GetElseBody());
        GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
        GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);
    }

    // merge
    GetBuilder().SetBlock(mergeBB);
}

void IRGenerator::Walk(MatchStatement* node) {
    IRJumpTableOp::CaseTable cases;
    BasicBlock* defaultBB = nullptr;
    auto maxStmts = node->GetStatements().size();
    std::vector<BasicBlock*> blocks(maxStmts);

    std::generate(blocks.begin(), blocks.end(), [&]() {
        auto* block = GetBuilder().CreateNewBlock();
        GetBuilder().GetCurrentBlock().AddSuccessor(block);
        return block;
    });

    auto value = EvaluateExpr(node->GetCondition());

    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();

    for (auto index = 0; index < maxStmts; ++index) {
        auto* labeled = node->GetStatements()[index]->As<LabeledStatement>();
        NEVER_BE_NULL(labeled);

        BasicBlock* caseBB = blocks[index];
        BasicBlock* fallthroughBB = (index + 1) < maxStmts ? blocks[index + 1] : mergeBB;

        PushLoopInfo(mergeBB, fallthroughBB);
        {
            auto* old = GetBuilder().SetBlock(caseBB);
            Walk(labeled);

            // do not insert branch if last statement is one of continue, break and pass statement.
            if (GetBuilder().GetCurrentBlock().empty() || !GetBuilder().GetCurrentBlock().back()->IsTerminal()) {
                GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
                GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);
            }

            GetBuilder().SetBlock(old); // restore block
        }
        PopLoopInfo();

        // add cases to CaseTable
        for (auto* label : labeled->GetLabels()) {
            if (label->IsDefault()) {
                MUST_BE_NULL(defaultBB);
                defaultBB = caseBB;
            }
            else {
                cases.emplace_back(label->GetEvaluatedCaseValue(), caseBB);
            }
        }
    }

    if (defaultBB == nullptr) {
        // Match statement may not have default label.
        // If it does, just jump into merge point.
        defaultBB = mergeBB;
    }

    GetBuilder().InsertBack<IRJumpTableOp>(node->GetMatchPosition(), value, defaultBB, std::move(cases));

    GetBuilder().SetBlock(mergeBB);
}

void IRGenerator::Walk(LabeledStatement* node) {
    // Skip on labels
    for (auto* stmt : node->GetNodes()) {
        super::Walk(stmt);
    }
}

void IRGenerator::Walk(ForStatement* node) {
    static auto HasNext = Value{ std::string{ "HasNext" } };
    static auto Next = Value{ std::string{ "Next" } };
    /*
     * Desugar for-loop into while-loop
     *
     * before:
     *      for <var> in <expr> <body>
     *
     * after:
     *      temp = <expr>
     *      while temp.HasNext {
     *          var <var> = temp.Next
     *          <body>
     *      }
     */
    BasicBlock* condBB = GetBuilder().CreateNewBlock();
    BasicBlock* bodyBB = GetBuilder().CreateNewBlock();
    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();

    auto it = EvaluateExpr(node->GetExpression());
    GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), condBB);
    GetBuilder().GetCurrentBlock().AddSuccessor(condBB);

    // cond
    {
        // Originally, conditional block is not to be sealed, but it's known that <var> is always
        // assigned at this point. So, it is safe to seal conditional block.
        GetBuilder().SetBlock(condBB);

        auto cond = GetBuilder().GetNewRegister();
        GetBuilder().InsertBack<IRGetMemberOp>(SourcePosition(), cond, it, HasNext);
        GetBuilder().InsertBack<IRConditionalBranchOp>(SourcePosition(), cond, bodyBB, mergeBB);
        GetBuilder().GetCurrentBlock().AddSuccessors({ bodyBB, mergeBB });
    }

    // body
    {
        PushLoopInfo(mergeBB, condBB);
        GetBuilder().SetBlock(bodyBB);

        auto value = GetBuilder().GetNewRegister();
        GetBuilder().InsertBack<IRGetMemberOp>(SourcePosition(), value, it, Next);

        GetBuilder().WriteSymbol(SourcePosition(), node->GetParameter()->GetSymbol(), value);

        super::Walk(node->GetBody());
        GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), condBB);
        GetBuilder().GetCurrentBlock().AddSuccessor(condBB);

        PopLoopInfo();
    }

    GetBuilder().SetBlock(mergeBB);
}

void IRGenerator::Walk(WhileStatement* node) {
    BasicBlock* condBB = GetBuilder().CreateNewBlock();
    BasicBlock* bodyBB = GetBuilder().CreateNewBlock();
    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();

    GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), condBB);
    GetBuilder().GetCurrentBlock().AddSuccessor(condBB);


    // cond
    {
        GetBuilder().SetBlock(condBB, /*sealBlock=*/false);

        auto cond = EvaluateExpr(node->GetCondition());
        GetBuilder().InsertBack<IRConditionalBranchOp>(SourcePosition(), cond, bodyBB, mergeBB);
        GetBuilder().GetCurrentBlock().AddSuccessors({ bodyBB, mergeBB });
    }

    // body
    {
        PushLoopInfo(mergeBB, condBB);
        GetBuilder().SetBlock(bodyBB);

        super::Walk(node->GetBody());
        GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), condBB);
        GetBuilder().GetCurrentBlock().AddSuccessor(condBB);

        PopLoopInfo();
    }

    GetBuilder().SealBlock(condBB);
    GetBuilder().SetBlock(mergeBB);
}

void IRGenerator::Walk(WithStatement* node) {
    static auto Close = Value{ std::string{ "Close" } };
    // Desugar statement: convert with statement to try-finally statement.
    // with open() as file {
    //     file.write("")
    // }
    // to
    // try {
    //     file = open()
    //     file.write("")
    // } finally {
    //     file.Close()
    // }

    BasicBlock* bodyBB = GetBuilder().CreateNewBlock();
    BasicBlock* finallyBB = GetBuilder().CreateNewBlock();
    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();
    ExceptInfo info(bodyBB, mergeBB);
    info.Handlers.emplace_back(finallyBB, nullptr);

    IRValue value;
    GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), bodyBB);
    GetBuilder().GetCurrentBlock().AddSuccessor(bodyBB);

    // try
    {
        GetBuilder().SetBlock(bodyBB);
        GetBuilder().GetCurrentBlock().AddSuccessor(finallyBB);

        value = EvaluateExpr(node->GetExpression());

        if (node->HasCapture()) {
            GetBuilder().WriteSymbol(SourcePosition(), node->GetCapture()->GetSymbol(), value);
        }

        super::Walk(node->GetBody());
        GenerateInvoke(SourcePosition(), value, Close, {});

        GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
        GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);
    }

    // finally
    {
        auto* symbol = new (m_context.GetAllocator()) VariableSymbol("<unnamed>", SourcePosition(), VariableType::Except, /*readonly=*/true);

        GetBuilder().SetBlock(finallyBB);
        GenerateInvoke(SourcePosition(), value, Close, {});

        // re-raise exception
        auto throwable = GetBuilder().GetNewRegister();
        GetBuilder().InsertBack<IRLoadSymbolOp>(SourcePosition(), throwable, symbol);
        GetBuilder().InsertBack<IRRaiseOp>(SourcePosition(), throwable);
    }

    GetBuilder().SetBlock(mergeBB);
    GetBuilder().AddExceptInfo(std::move(info));
}

void IRGenerator::Walk(TryStatement* node) {
    BasicBlock* bodyBB = GetBuilder().CreateNewBlock();
    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();
    FinallyStatement* finally_ = nullptr;
    BasicBlock* endBB;

    GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), bodyBB);
    GetBuilder().GetCurrentBlock().AddSuccessor(bodyBB);

    auto it = std::find_if(node->GetHandlers().begin(), node->GetHandlers().end(), [](Statement* handler) {
        return handler->As<FinallyStatement>() != nullptr;
    });

    if (it != node->GetHandlers().end()) {
        finally_ = (*it)->As<FinallyStatement>();
        NEVER_BE_NULL(finally_);
    }

    // body
    {
        GetBuilder().SetBlock(bodyBB);
        GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);

        super::Walk(node->GetBody());

        if (finally_ != nullptr) { super::Walk(finally_); }

        GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
        endBB = &(GetBuilder().GetCurrentBlock());
    }

    ExceptInfo info(bodyBB, endBB);

    // except
    for (auto* handler : node->GetHandlers()) {
        if (auto* except = handler->As<ExceptStatement>()) {
            BasicBlock* handlerBB = GetBuilder().CreateNewBlock();

            bodyBB->AddSuccessorOnly(handlerBB);
            GetBuilder().SetBlock(handlerBB);

            super::Walk(except->GetBody());

            if (finally_ != nullptr) { super::Walk(finally_); }

            info.Handlers.emplace_back(handlerBB, except->GetTypeSymbol());
            GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
            GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);
        }
    }

    if (finally_ != nullptr) {
        // insert finally as handler re-raising caught exception.
        auto* symbol = new (m_context.GetAllocator()) VariableSymbol("<unnamed>", SourcePosition(), VariableType::Except, /*readonly=*/true);
        BasicBlock* handlerBB = GetBuilder().CreateNewBlock();

        bodyBB->AddSuccessor(handlerBB);
        GetBuilder().SetBlock(handlerBB);

        super::Walk(finally_->GetBody());

        info.Handlers.emplace_back(handlerBB, nullptr);

        // re-raise exception
        auto throwable = GetBuilder().GetNewRegister();
        GetBuilder().InsertBack<IRLoadSymbolOp>(SourcePosition(), throwable, symbol);
        GetBuilder().InsertBack<IRRaiseOp>(SourcePosition(), throwable);
    }

    GetBuilder().SetBlock(mergeBB);
    GetBuilder().AddExceptInfo(std::move(info));
}

void IRGenerator::Walk(ExceptStatement* node) {
    // Do nothing. Handled by TryStatement.
}

void IRGenerator::Walk(FinallyStatement* node) {
    // Do nothing. Handled by TryStatement.
}

void IRGenerator::Walk(BreakStatement* node) {
    if (node->IsConditional()) {
        auto cond = EvaluateExpr(node->GetCondition());
        BasicBlock* mergeBB = GetBuilder().CreateNewBlock();

        GetBuilder().InsertBack<IRConditionalBranchOp>(node->GetBreakPosition(), cond, GetNearestLoop().Break, mergeBB);
        GetBuilder().GetCurrentBlock().AddSuccessors({ GetNearestLoop().Break, mergeBB });
        GetBuilder().SetBlock(mergeBB);
    }
    else {
        GetBuilder().InsertBack<IRBranchOp>(node->GetBreakPosition(), GetNearestLoop().Break);
        GetBuilder().GetCurrentBlock().AddSuccessor(GetNearestLoop().Break);
    }
}

void IRGenerator::Walk(ContinueStatement* node) {
    if (node->IsConditional()) {
        auto cond = EvaluateExpr(node->GetCondition());
        BasicBlock* mergeBB = GetBuilder().CreateNewBlock();

        GetBuilder().InsertBack<IRConditionalBranchOp>(node->GetContinuePosition(), cond, GetNearestLoop().Continue,
                                                       mergeBB);
        GetBuilder().GetCurrentBlock().AddSuccessors({ GetNearestLoop().Continue, mergeBB });
        GetBuilder().SetBlock(mergeBB);
    }
    else {
        GetBuilder().InsertBack<IRBranchOp>(node->GetContinuePosition(), GetNearestLoop().Continue);
        GetBuilder().GetCurrentBlock().AddSuccessor(GetNearestLoop().Continue);
    }
}

void IRGenerator::Walk(ReturnStatement* node) {
    auto ret = node->HasReturnValue() ? EvaluateExpr(node->GetReturnValue()) : IRInvalidValue;

    GetBuilder().InsertBack<IRReturnOp>(node->GetReturnPosition(), ret);
}

void IRGenerator::Walk(AssertStatement* node) {
    BasicBlock* assertBB = GetBuilder().CreateNewBlock();
    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();
    auto cond = EvaluateExpr(node->GetCondition());

    GetBuilder().InsertBack<IRConditionalBranchOp>(node->GetAssertPosition(), cond, mergeBB, assertBB);
    GetBuilder().GetCurrentBlock().AddSuccessors({ mergeBB, assertBB });

    // assert
    {
        GetBuilder().SetBlock(assertBB);

        IRValue message;

        if (node->HasMessage()) {
            message = EvaluateExpr(node->GetMessage());
        }
        else {
            static auto Message = Value{ std::string{ "assertion failed." } };
            message = GetBuilder().ReadConst(SourcePosition(), Message);
        }

        GetBuilder().InsertBack<IRAssertOp>(node->GetAssertPosition(), message);
    }

    GetBuilder().SetBlock(mergeBB);
}

void IRGenerator::Walk(PassStatement* node) {
    // do nothing.
}

inline IROpCode Assign2IR(AssignOp op) {
    switch (op) {
        case AssignOp::Assign: { NOT_REACHABLE; }
        case AssignOp::Add: return IROpCode::Add;
        case AssignOp::Sub: return IROpCode::Sub;
        case AssignOp::Mul: return IROpCode::Mul;
        case AssignOp::Div: return IROpCode::Div;
        case AssignOp::Mod: return IROpCode::Mod;
        case AssignOp::BitAnd: return IROpCode::And;
        case AssignOp::BitOr: return IROpCode::Or;
        case AssignOp::BitXor: return IROpCode::Xor;
        case AssignOp::LeftShift: return IROpCode::Shl;
        case AssignOp::RightShift: return IROpCode::Shr;
    }
}

void IRGenerator::Walk(AssignStatement* node) {
    switch (node->GetTarget()->GetKind()) {
        case ExpressionKind::Literal: {
            auto* literal = node->GetTarget()->As<LiteralExpression>();
            NEVER_BE_NULL(literal);
            assert((literal->GetLiteralType() == LiteralType::Variable) && "Must be rejected at SemanticChecker.");

            auto value = EvaluateExpr(node->GetValue());

            if (node->GetOp() != AssignOp::Assign) {
                auto target = GetBuilder().ReadSymbol(literal->GetPosition(), literal->GetSymbol());
                auto ret = GetBuilder().GetNewRegister();
                GetBuilder().InsertBack<IRInplaceOp>(Assign2IR(node->GetOp()), node->GetOpPosition(), ret, target, value);

                value = ret;
            }

            GetBuilder().WriteSymbol(node->GetOpPosition(), literal->GetSymbol(), value);
            break;
        }

        case ExpressionKind::MemberAccess: {
            auto* member = node->GetTarget()->As<MemberAccessExpression>();
            NEVER_BE_NULL(member);

            auto target = EvaluateExpr(member->GetTarget());
            auto value = EvaluateExpr(node->GetValue());
            auto name = Value{ member->GetMemberName().GetString() };

            if (node->GetOp() != AssignOp::Assign) {
                auto t = GetBuilder().GetNewRegister();
                GetBuilder().InsertBack<IRGetMemberOp>(member->GetDotPosition(), t, target, name);

                auto newVal = GetBuilder().GetNewRegister();
                GetBuilder().InsertBack<IRInplaceOp>(Assign2IR(node->GetOp()), node->GetOpPosition(), newVal, t, value);
                value = newVal;
            }

            GetBuilder().InsertBack<IRSetMemberOp>(node->GetOpPosition(), target, name, value);
            break;
        }

        case ExpressionKind::Subscript: {
            auto* subscript = node->GetTarget()->As<SubscriptExpression>();
            NEVER_BE_NULL(subscript);

            auto target = EvaluateExpr(subscript->GetTarget());
            auto index = EvaluateExpr(subscript->GetIndex());
            auto value = EvaluateExpr(node->GetValue());

            if (node->GetOp() != AssignOp::Assign) {
                auto t = GetBuilder().GetNewRegister();
                GetBuilder().InsertBack<IRGetSubscriptOp>(subscript->GetOpenSquarePosition(), t, target, index);

                auto newVal = GetBuilder().GetNewRegister();
                GetBuilder().InsertBack<IRInplaceOp>(Assign2IR(node->GetOp()), node->GetOpPosition(), newVal, t, value);
                value = newVal;
            }

            GetBuilder().InsertBack<IRSetSubscriptOp>(node->GetOpPosition(), target, index, value);
            break;
        }

        default:
            NOT_REACHABLE; // Must be rejected at SemanticChecker.
    }
}