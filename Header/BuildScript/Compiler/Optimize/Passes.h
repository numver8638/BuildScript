/*
 * Passes.h
 * - Define optimization passes.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_OPTIMIZE_PASSES_H
#define BUILDSCRIPT_COMPILER_OPTIMIZE_PASSES_H

#include <vector>

namespace BuildScript {
    class Context;
    class IRCodeBlock;

    namespace OptimizerPass {
        void ConstantPropagation(Context&, IRCodeBlock&);
        void CommonSubexpressionElimination(Context&, IRCodeBlock&);
        void SimplifyBranches(Context&, IRCodeBlock&);
        void LoopInvariantMove(Context&, IRCodeBlock&);
        void RemoveRedundantBranches(Context&, IRCodeBlock&);
    } // end namespace OptimizerPass
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_OPTIMIZE_PASSES_H