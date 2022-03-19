/*
 * CompileOptions.h
 * - Set of compiler options.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_COMPILEOPTIONS_H
#define BUILDSCRIPT_COMPILER_COMPILEOPTIONS_H

#include <cassert>

#include <BuildScript/Utils/Encoding.h>

namespace BuildScript {
    /**
     * @brief Represents optimization level.
     */
    enum class OptimizeLevel {
        None,       //!< Disable optimization.
        Optimize,   //!< Enable optimization.
    }; // end enum OptimizeLevel

    /**
     * @brief Convert integer value to @c OptimizeLevel.
     * @return @c OptimizeLevel corresponds to @c level.
     */
    OptimizeLevel IntToLevel(int level) {
        if (level < 0) { level = 0; }

        if (level > static_cast<int>(OptimizeLevel::Optimize)) {
            return OptimizeLevel::Optimize;
        }
        else {
            return static_cast<OptimizeLevel>(level);
        }
    }

    /**
     * @brief Set of compiler options.
     */
    struct CompileOptions {
        bool DumpAST = false;                                   //!< Dump AST in stdout.
        bool DumpIR = false;                                    //!< Dump IR in stdout.
        bool DumpBytecode = false;                              //!< Dump bytecode in stdout.

        bool GenerateDebugInfo = false;                         //!< Generate debug information for debugging.

        bool SyntaxOnly = false;                                //!< Check syntax only and do not compile.

        Encoding* Encoding = &Encoding::UTF8();                 //!< Encoding of source text.

        unsigned TabSize = 4;                                   //!< Tab size for column tracking.

        OptimizeLevel OptimizeLevel = OptimizeLevel::Optimize;  //!< Optimize bytecode.
    }; // end struct CompileOptions
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_COMPILEOPTIONS_H