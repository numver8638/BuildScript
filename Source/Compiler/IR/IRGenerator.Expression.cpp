/*
 * IRGenerator.Expression.cpp
 * - Intermediate Representation code generator.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRGenerator.h>

#include <algorithm>
#include <iterator>

#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/AST/ParameterList.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>
#include <BuildScript/Compiler/IR/Statements.h>

using namespace BuildScript;

void IRGenerator::Walk(InvalidExpression* node) {
    NEVER_BE_CALLED("cannot be generated with erroneous script.");
}

void IRGenerator::Walk(PassExpression* node) {
    auto ret = GetBuilder().ReadConst(SourcePosition(), Value{ None });

    SetReturn(ret);
}

void IRGenerator::Walk(TernaryExpression* node) {
    IRValue onTrue, onFalse;
    BasicBlock* onTrueBB = GetBuilder().CreateNewBlock();
    BasicBlock* onFalseBB = GetBuilder().CreateNewBlock();
    BasicBlock* mergeBB = GetBuilder().CreateNewBlock();

    auto cond = EvaluateExpr(node->GetCondition());
    GetBuilder().InsertBack<IRConditionalBranchOp>(SourcePosition(), cond, onTrueBB, onFalseBB);
    GetBuilder().GetCurrentBlock().AddSuccessors({ onTrueBB, onFalseBB });

    // OnTrue
    GetBuilder().SetBlock(onTrueBB);
    onTrue = EvaluateExpr(node->GetValueOnTrue());
    GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
    GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);

    // OnFalse
    GetBuilder().SetBlock(onFalseBB);
    onFalse = EvaluateExpr(node->GetValueOnFalse());
    GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
    GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);

    // Merge
    GetBuilder().SetBlock(mergeBB);
    auto ret = GetBuilder().GetNewRegister();
    GetBuilder().InsertBack<IRSelectOp>(ret, std::vector<IRValue>{ onTrue, onFalse });

    SetReturn(ret);
}

static inline std::pair<IROpCode, IRTestKind> Convert(BinaryOp op) {
    switch (op) {
        case BinaryOp::Mul:             return { IROpCode::Mul,  IRTestKind::Less };
        case BinaryOp::Div:             return { IROpCode::Div,  IRTestKind::Less };
        case BinaryOp::Mod:             return { IROpCode::Mod,  IRTestKind::Less };
        case BinaryOp::Add:             return { IROpCode::Add,  IRTestKind::Less };
        case BinaryOp::Sub:             return { IROpCode::Sub,  IRTestKind::Less };
        case BinaryOp::LeftShift:       return { IROpCode::Shl,  IRTestKind::Less };
        case BinaryOp::RightShift:      return { IROpCode::Shr,  IRTestKind::Less };
        case BinaryOp::BitAnd:          return { IROpCode::And,  IRTestKind::Less };
        case BinaryOp::BitXor:          return { IROpCode::Xor,  IRTestKind::Less };
        case BinaryOp::BitOr:           return { IROpCode::Or,   IRTestKind::Less };
        case BinaryOp::Less:            return { IROpCode::Test, IRTestKind::Less };
        case BinaryOp::LessOrEqual:     return { IROpCode::Test, IRTestKind::LessOrEqual };
        case BinaryOp::Grater:          return { IROpCode::Test, IRTestKind::Grater };
        case BinaryOp::GraterOrEqual:   return { IROpCode::Test, IRTestKind::GraterOrEqual };
        case BinaryOp::Equal:           return { IROpCode::Test, IRTestKind::Equal };
        case BinaryOp::NotEqual:        return { IROpCode::Test, IRTestKind::NotEqual };

        case BinaryOp::LogicalAnd:
        case BinaryOp::LogicalOr:
            NOT_REACHABLE;
    }
}

void IRGenerator::Walk(BinaryExpression* node) {
    IRValue ret;

    // Logical "AND" and "OR" are lazy evaluated.
    if (node->GetOp() == BinaryOp::LogicalAnd || node->GetOp() == BinaryOp::LogicalOr) {
        BasicBlock* rightBB = GetBuilder().CreateNewBlock();
        BasicBlock* mergeBB = GetBuilder().CreateNewBlock();
        IRValue left, right;
        left = EvaluateExpr(node->GetLeft());

        // if left is false on "AND" or true on "OR" skip right hand side expression.
        if (node->GetOp() == BinaryOp::LogicalAnd) {
            GetBuilder().InsertBack<IRConditionalBranchOp>(node->GetOpPosition(), left, rightBB, mergeBB);
            GetBuilder().GetCurrentBlock().AddSuccessors({rightBB, mergeBB});
        }
        else /*(node->GetOp() == BinaryOp::LogicalOr)*/ {
            GetBuilder().InsertBack<IRConditionalBranchOp>(node->GetOpPosition(), left, mergeBB, rightBB);
            GetBuilder().GetCurrentBlock().AddSuccessors({mergeBB, rightBB});
        }

        // right
        GetBuilder().SetBlock(rightBB);
        right = EvaluateExpr(node->GetRight());
        GetBuilder().InsertBack<IRBranchOp>(SourcePosition(), mergeBB);
        GetBuilder().GetCurrentBlock().AddSuccessor(mergeBB);

        // merge
        GetBuilder().SetBlock(mergeBB);
        ret = GetBuilder().GetNewRegister();
        GetBuilder().InsertBack<IRSelectOp>(ret, std::vector<IRValue>{ left, right });
    }
    else {
        auto left = EvaluateExpr(node->GetLeft());
        auto right = EvaluateExpr(node->GetRight());
        ret = GetBuilder().GetNewRegister();

        auto [op, test] = Convert(node->GetOp());
        if (op != IROpCode::Test) {
            GetBuilder().InsertBack<IRBinaryOp>(op, node->GetOpPosition(), ret, left, right);
        }
        else /*(op == IROpCode::Test)*/ {
            GetBuilder().InsertBack<IRTestOp>(node->GetOpPosition(), ret, test, left, right);
        }
    }

    SetReturn(ret);
}

void IRGenerator::Walk(TypeTestExpression* node) {
    auto kind = node->IsNegative() ? IRTestKind::NotInstance : IRTestKind::Instance;
    auto ret = GetBuilder().GetNewRegister();
    auto target = EvaluateExpr(node->GetTarget());

    // Get type object
    auto type = GetBuilder().ReadSymbol(node->GetTypename().GetPosition(), node->GetTypeSymbol());

    GetBuilder().InsertBack<IRTestOp>(node->GetIsPosition(), ret, kind, target, type);
    SetReturn(ret);
}

void IRGenerator::Walk(ContainmentTestExpression* node) {
    auto kind = node->IsNegative() ? IRTestKind::NotContain : IRTestKind::Contain;
    auto ret = GetBuilder().GetNewRegister();
    auto target = EvaluateExpr(node->GetTarget());
    auto value = EvaluateExpr(node->GetValue());

    GetBuilder().InsertBack<IRTestOp>(node->GetInPosition(), ret, kind, target, value);
    SetReturn(ret);
}

void IRGenerator::Walk(UnaryExpression* node) {
    auto target = EvaluateExpr(node->GetExpression());
    IRValue ret;

    if (node->GetOp() == UnaryOp::Identity) {
        ret = target;
    }
    else {
        ret = GetBuilder().GetNewRegister();
        auto op = (node->GetOp() == UnaryOp::Negate) ? IROpCode::Neg : IROpCode::Not;

        GetBuilder().InsertBack<IRUnaryOp>(op, node->GetOpPosition(), ret, target);
    }

    SetReturn(ret);
}

void IRGenerator::Walk(DefinedExpression* node) {
    auto ret = GetBuilder().GetNewRegister();
    auto target = node->HasTarget() ? EvaluateExpr(node->GetTarget()) : IRInvalidValue;
    auto id = Value{ node->GetID().GetString() };
    GetBuilder().InsertBack<IRDefinedOp>(node->GetDefinedPosition(), ret, std::move(id), target);

    SetReturn(ret);
}

void IRGenerator::Walk(RaiseExpression* node) {
    auto target = EvaluateExpr(node->GetTarget());
    GetBuilder().InsertBack<IRRaiseOp>(node->GetRaisePosition(), target);

    SetReturn(IRInvalidValue);  // Never returns.
}

void IRGenerator::Walk(MemberAccessExpression* node) {
    // SET_MEMBER is handled in AssignmentStatement.
    auto target = EvaluateExpr(node->GetTarget());
    auto ret = GetBuilder().GetNewRegister();
    auto member = Value{ node->GetMemberName().GetString() };

    GetBuilder().InsertBack<IRGetMemberOp>(node->GetDotPosition(), ret, target, std::move(member));
    SetReturn(ret);
}

void IRGenerator::Walk(InvocationExpression* node) {
    IRValue ret;
    auto pos = node->GetOpenParenPosition();
    auto IsOneOfMethod = [](Expression* target) -> bool {
        auto* literal = target->As<LiteralExpression>();

        if (literal == nullptr || literal->GetLiteralType() != LiteralType::Variable) { return false; }

        auto* method = literal->GetSymbol()->As<MethodSymbol>();
        return (method != nullptr) && !method->IsStatic();
    };
    auto IsInitCall = [](Expression* target) -> bool {
        if (auto* literal = target->As<LiteralExpression>()) {
            auto type = literal->GetLiteralType();
            return (type == LiteralType::Self) || (type == LiteralType::Super);
        }
        return false;
    };
    auto IsSelf = [](Expression* target) -> bool {
        auto* literal = target->As<LiteralExpression>();
        return (literal != nullptr) && (literal->GetLiteralType() == LiteralType::Self);
    };

    if (auto* member = node->GetTarget()->As<MemberAccessExpression>()) {
        // invocation is method invocation
        auto target = EvaluateExpr(member->GetTarget());
        auto name = Value{ member->GetMemberName().GetString() };
        ret = GenerateInvoke(pos, target, std::move(name), node->GetArguments());
    }
    else if (IsOneOfMethod(node->GetTarget())) {
        // invocation is method invocation of self.
        auto target = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSelf());
        auto name = Value{ std::string{ node->GetTarget()->As<LiteralExpression>()->AsString() } };
        ret = GenerateInvoke(pos, target, std::move(name), node->GetArguments());
    }
    else if (IsInitCall(node->GetTarget())) {
        auto isSelf = IsSelf(node->GetTarget());
        // invocation is initializer call
        auto target = GetBuilder().ReadSymbol(SourcePosition(),
                                              isSelf ? VariableSymbol::GetSelf() : VariableSymbol::GetSuper());
        auto name = Value{ std::string{ MethodSymbol::InitializerName } };
        ret = GenerateInvoke(pos, target, std::move(name), node->GetArguments());
    }
    else {
        // invocation is function/closure invocation
        auto target = EvaluateExpr(node->GetTarget());
        ret = GenerateCall(pos, target, node->GetArguments());
    }

    SetReturn(ret);
}

void IRGenerator::Walk(SubscriptExpression* node) {
    // SET_SUBSCRIPT is handled in AssignmentStatement.
    auto target = EvaluateExpr(node->GetTarget());
    auto index = EvaluateExpr(node->GetIndex());
    auto ret = GetBuilder().GetNewRegister();

    GetBuilder().InsertBack<IRGetSubscriptOp>(node->GetOpenSquarePosition(), ret, target, index);
    SetReturn(ret);
}

void IRGenerator::Walk(ParenthesizedExpression* node) {
    super::Walk(node);
}

void IRGenerator::Walk(ListExpression* node) {
    std::vector<IRValue> items;
    auto it = node->GetItems();

    items.resize(it.size());
    std::transform(it.begin(), it.end(), items.begin(), [&](auto* e) { return EvaluateExpr(e); });

    auto ret = GetBuilder().GetNewRegister();
    auto pos = node->GetOpenSquarePosition();

    GetBuilder().InsertBack<IRMakeOp>(IROpCode::MakeList, pos, ret, std::move(items));
    SetReturn(ret);
}

void IRGenerator::Walk(MapExpression* node) {
    std::vector<IRValue> items;

    for (auto& [key, _, value] : node->GetItems()) {
        items.push_back(EvaluateExpr(key));
        items.push_back(EvaluateExpr(value));
    }

    auto ret = GetBuilder().GetNewRegister();
    auto pos = node->GetOpenBracePosition();
    GetBuilder().InsertBack<IRMakeOp>(IROpCode::MakeMap, pos, ret, std::move(items));
    SetReturn(ret);
}

void IRGenerator::Walk(ClosureExpression* node) {
    {
        std::vector<Symbol*> args;
        bool vararg = node->GetParameterList()->HasVariadicArgument();
        auto params = node->GetParameterList()->GetParameters();

        std::transform(params.begin(), params.end(), std::back_inserter(args), [](auto* param) {
            return param->GetSymbol();
        });

        PushBuilder(node->GetSymbol()->GetMangledName(), args, vararg);

        GenerateBody(node->GetBody());

        PopBuilder();
    }

    std::vector<IRValue> args;

    args.push_back(GetBuilder().ReadSymbol(SourcePosition(), node->GetSymbol()));

    for (auto* symbol : node->GetBoundedLocals()) {
        args.push_back(GetBuilder().ReadSymbol(SourcePosition(), symbol));
    }

    auto ret = GetBuilder().GetNewRegister();

    GetBuilder().InsertBack<IRMakeOp>(IROpCode::MakeClosure, node->GetArrowPosition(), ret, std::move(args));
    SetReturn(ret);
}

void IRGenerator::Walk(LiteralExpression* node) {
    auto IsClassMember = [](Symbol* symbol) {
        if (auto* method = symbol->As<MethodSymbol>()) { return !method->IsStatic(); }

        if (auto* property = symbol->As<PropertySymbol>()) { return true; }

        return false;
    };
    auto ret = IRInvalidValue;

    switch (node->GetLiteralType()) {
        case LiteralType::None:
            ret = GetBuilder().ReadConst(node->GetPosition(), Value{None});
            break;

        case LiteralType::Variable:
            if (IsClassMember(node->GetSymbol())) {
                ret = GetBuilder().GetNewRegister();
                auto self = GetBuilder().ReadSymbol(SourcePosition(), VariableSymbol::GetSelf());
                GetBuilder().InsertBack<IRGetMemberOp>(node->GetPosition(), ret, self, node->GetRawValue());
                break;
            }
            [[fallthrough]];

        case LiteralType::Self:
        case LiteralType::Super:
            ret = GetBuilder().ReadSymbol(node->GetPosition(), node->GetSymbol());
            break;

        case LiteralType::Integer:
        case LiteralType::Float:
        case LiteralType::Boolean:
        case LiteralType::String: {
            auto value = node->GetRawValue();

            if (node->GetLiteralType() == LiteralType::String && node->HasInterpolations()) {
                // Desugar expression: change interpolation to `"...".Format(...)`.
                static auto Format = Value{ std::string{ "Format" } };
                auto reg = GetBuilder().ReadConst(node->GetPosition(), value);

                ret = GenerateInvoke(SourcePosition(), reg, Format, node->GetInterpolations());
            }
            else {
                ret = GetBuilder().ReadConst(node->GetPosition(), value);
            }

            break;
        }
    }

    SetReturn(ret);
}