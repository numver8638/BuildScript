/*
 * AccessFlags.h
 * - Represents accessibility of variables or fields.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_ACCESSFLAGS_H
#define BUILDSCRIPT_COMPILER_AST_ACCESSFLAGS_H

namespace BuildScript {
    /**
     * @brief Represents accessibility of variables or fields.
     */
    enum class AccessFlags {
        Invalid,    ///!< Invalid modifier or modifier do not exists.
        ReadWrite,  ///!< Represents `var` - can be read and written.
        Static,     ///!< Represents `static` - field has static storage.
        Const       ///!< Represents 'const' - only can be read.
    }; // end enum AccessFlags

    /**
     * @brief Convert @c AccessFlags to corresponding keyword.
     * @param flags an @c AccessFlags to convert.
     * @return a @c std::string_view representing keyword.
     */
    inline std::string_view AccessFlagsToKeyword(AccessFlags flags) {
        static const std::string_view table[] = {
            "<invalid>",
            "var",
            "static",
            "const"
        };

        return table[static_cast<size_t>(flags)];
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_ACCESSFLAGS_H