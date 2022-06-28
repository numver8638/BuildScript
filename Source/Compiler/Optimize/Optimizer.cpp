/*
 * Optimizer.cpp
 * - Optimize IR codes.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Optimize/Optimizer.h>

#include <BuildScript/Compiler/Optimize/Passes.h>

using namespace BuildScript;
Optimizer::Optimizer(Context& context)
    : m_context(context) {}

void Optimizer::Optimize(std::vector<IRCodeBlock>& codes) {
    using namespace BuildScript::OptimizerPass;

    for (auto& code : codes) {
        ConstantPropagation(m_context, code);
        CommonSubexpressionElimination(m_context, code);
        LoopInvariantMove(m_context, code);
        SimplifyBranches(m_context, code);
        RemoveRedundantBranches(m_context, code);
    }
}