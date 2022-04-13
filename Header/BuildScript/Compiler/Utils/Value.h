/*
 * Value.h
 * - Represents constant value evaluated during compile.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_UTILS_VALUE_H
#define BUILDSCRIPT_COMPILER_UTILS_VALUE_H

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace BuildScript {
    /**
     * @brief Represents constant value evaluated during compile.
     */
    using Value = std::optional<std::variant<std::monostate, bool, int64_t, double, std::string>>;

    constexpr auto None = std::monostate();
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_UTILS_VALUE_H