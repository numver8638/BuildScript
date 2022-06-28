/*
 * IRGenerator.h
 * - Intermediate Representation code generator.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IR_IRGENERATOR_H
#define BUILDSCRIPT_COMPILER_IR_IRGENERATOR_H

#include <algorithm>
#include <stack>
#include <vector>
#include <list>
#include <iterator>

#include <BuildScript/Assert.h>
#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/AST/ASTWalker.h>
#include <BuildScript/Compiler/AST/Nodes.def>
#include <BuildScript/Compiler/IR/IRCodeBlock.h>
#include <BuildScript/Compiler/IR/IRBuilder.h>
#include <BuildScript/Compiler/IR/IRStatement.h>
#include <BuildScript/Compiler/Utils/Value.h>
#include <BuildScript/Utils/TrailObjects.h>

namespace BuildScript {
    class Context;          // Defined in <BuildScript/Compiler/Basic/Context.h>
    class ErrorReporter;    // Defined in <BuildScript/Compiler/Basic/ErrorReporter.h>
    class IRBuilder;        // Defined in <BuildScript/Compiler/IR/IRBuilder.h>

    /**
     * @brief Intermediate Representation code generator.
     */
    class IRGenerator final : ASTWalker {
    private:
        using super = ASTWalker;

        struct LoopInfo {
            BasicBlock* Break;
            BasicBlock* Continue;

            LoopInfo(BasicBlock* B, BasicBlock* C)
                : Break(B), Continue(C) {}
        }; // end struct LoopInfo

        Context& m_context;
        ErrorReporter& m_reporter;

        std::stack<IRBuilder> m_builderStack;
        std::vector<IRCodeBlock> m_codes;
        std::stack<LoopInfo> m_loopStack;

        IRValue m_return = IRInvalidValue;

        IRBuilder& GetBuilder() {
            assert(!m_builderStack.empty());

            return m_builderStack.top();
        }

        void PushBuilder(std::string name, const std::vector<Symbol*>& args, bool vararg);

        void PopBuilder();

        LoopInfo& GetNearestLoop() {
            assert(!m_loopStack.empty() && "never be called out of loop.");
            return m_loopStack.top();
        }

        void PushLoopInfo(BasicBlock* _break, BasicBlock* _continue) {
            m_loopStack.emplace(_break, _continue);
        }

        void PopLoopInfo() { m_loopStack.pop(); }

        IRValue EvaluateExpr(Expression* expr) {
            super::Walk(expr);
            return m_return;
        }

        void SetReturn(IRValue value) {
            m_return = value;
        }

        #define INSERT_EMPTY_RETURN \
            do { \
                auto reg = GetBuilder().GetNewRegister(); \
                GetBuilder().InsertBack<IRLoadConstOp>(SourcePosition(), reg, Value{ None }); \
                GetBuilder().InsertBack<IRReturnOp>(SourcePosition(), reg); \
            } while (false)

        void GenerateBody(Statement* body);

        IRValue GenerateInvoke(SourcePosition pos, IRValue target, Value member, std::vector<IRValue> args);
        IRValue
        GenerateInvoke(SourcePosition pos, IRValue target, Value member, TrailIterator<Expression*> exprs) {
            std::vector<IRValue> args;
            args.resize(exprs.size());
            std::transform(exprs.begin(), exprs.end(), args.begin(), [&](auto* e) { return EvaluateExpr(e); });

            return GenerateInvoke(pos, target, std::move(member), std::move(args));
        }

        IRValue GenerateCall(SourcePosition pos, IRValue target, std::vector<IRValue> args);
        IRValue GenerateCall(SourcePosition pos, IRValue target, TrailIterator<Expression*> exprs) {
            std::vector<IRValue> args;
            args.resize(exprs.size());
            std::transform(exprs.begin(), exprs.end(), args.begin(), [&](auto* e) { return EvaluateExpr(e); });

            return GenerateCall(pos, target, std::move(args));
        }

        NODE_LIST(DEFINE_WALK)

    public:
        IRGenerator(Context& context, ErrorReporter& reporter)
            : m_context(context), m_reporter(reporter) {}

        /**
         * @brief Generates IR code from the AST tree.
         * @param node a @c ASTNode to generate.
         * @return a list of @c IRCodeBlock.
         */
        std::vector<IRCodeBlock> GenerateIR(ASTNode* node) {
            super::Walk(node);
            return m_codes;
        }
    }; // end class IRGenerator
}

#endif // BUILDSCRIPT_COMPILER_IR_IRGENERATOR_H