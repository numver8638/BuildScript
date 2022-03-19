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
#include <string>
#include <string_view>
#include <variant>

namespace BuildScript {
    /**
     * @brief Represents constant value evaluated during compile.
     */
    struct Value : public std::variant<bool, int64_t, double, std::string> {
        using super = std::variant<bool, int64_t, double, std::string>;

        using super::variant;


        constexpr Value(int i)
            : variant(int64_t(i)) {}

        Value(std::string_view str)
            : variant(std::string(str)) {}
    }; // end struct Value
} // end namespace BuildScript

template <>
struct std::hash<BuildScript::Value> {
    size_t operator ()(const BuildScript::Value& val) const noexcept {
        return std::hash<BuildScript::Value::super>()(val);
    }
};

#endif // BUILDSCRIPT_COMPILER_UTILS_VALUE_H