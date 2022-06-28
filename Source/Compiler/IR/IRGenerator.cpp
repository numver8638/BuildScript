/*
 * IRGenerator.cpp
 * - Intermediate Representation code generator.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRGenerator.h>

#include <BuildScript/Compiler/IR/Statements.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/Symbol/Symbol.h>

using namespace BuildScript;

IRValue IRGenerator::GenerateInvoke(SourcePosition pos, IRValue target, Value member, std::vector<IRValue> args) {
    auto ret = GetBuilder().GetNewRegister();
    GetBuilder().InsertBack<IRInvokeOp>(pos, ret, target, std::move(member), std::move(args));

    return ret;
}

IRValue IRGenerator::GenerateCall(SourcePosition pos, IRValue target, std::vector<IRValue> args) {
    auto ret = GetBuilder().GetNewRegister();
    GetBuilder().InsertBack<IRCallOp>(pos, ret, target, std::move(args));

    return ret;
}

void IRGenerator::GenerateBody(Statement* body) {
    if (auto* block = body->As<BlockStatement>()) {
        super::Walk(block);

        // Check last block contains return statement.
        // If not, insert 'return' opcode.
        auto& current = GetBuilder().GetCurrentBlock();
        if (current.empty() || current.back()->GetOp() != IROpCode::Return) {
            INSERT_EMPTY_RETURN;
        }
    }
    else if (auto* arrow = body->As<ArrowStatement>()) {
        auto value = EvaluateExpr(arrow->GetExpression());
        GetBuilder().InsertBack<IRReturnOp>(SourcePosition(), value);
    }
    else {
        NOT_REACHABLE;
    }
}

void IRGenerator::PushBuilder(std::string name, const std::vector<Symbol*>& args, bool vararg) {
    m_builderStack.emplace(m_context, std::move(name), args, vararg);
}

void IRGenerator::PopBuilder()  {
    m_codes.push_back(GetBuilder().Finalize());
    m_builderStack.pop();
}