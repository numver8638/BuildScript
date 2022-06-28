/*
 * Optimizer.h
 * - Optimize IR codes.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_OPTIMIZE_OPTIMIZER_H
#define BUILDSCRIPT_COMPILER_OPTIMIZE_OPTIMIZER_H

#include <vector>

#include <BuildScript/Compiler/CompileOptions.h>
#include <BuildScript/Compiler/IR/IRCodeBlock.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    class ErrorReporter;
    class Context;

    /**
     * @brief Optimize IR codes.
     */
    class Optimizer final : NonCopyable {
    private:
        Context& m_context;

    public:
        explicit Optimizer(Context& context);

        void Optimize(std::vector<IRCodeBlock>& codes);
    }; // end class Optimizer
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_OPTIMIZE_OPTIMIZER_H