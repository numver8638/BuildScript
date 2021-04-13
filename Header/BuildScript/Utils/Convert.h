/*
 * Convert.h
 * - Utility functions for converting integers and floating point numbers.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_CONVERT_H
#define BUILDSCRIPT_UTILS_CONVERT_H

#include <string>
#include <tuple>

namespace BuildScript {
    /**
     * @brief Convert string to integer.
     * @param str string to convert.
     * @warning This function assumes that input is always well-formed and no leading whitespaces nor signs.
     * @return @c std::tuple which contains @c bool that indicates function returns successfully and @c int64_t that is
     *         converted value from input.
     */
    std::tuple<bool, int64_t> ConvertInteger(const std::string& str);

    /**
     * @brief Convert string to floating point number.
     * @param str string to convert.
     * @return @c std::tuple which contains @c bool that indicates function returns successfully and @c double that is
     *         converted value from input.
     * @warning This conversion function may return inexact value: not rounded correctly specified in IEEE 754.
     * @warning This function assumes that input is always well-formed and no leading whitespaces nor signs.
     * @TODO Make this function round correctly.
     */
    std::tuple<bool, double> ConvertFloat(const std::string& str);
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_CONVERT_H