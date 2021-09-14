/*
 * Functions.h
 * - Typedefs for std::function.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_FUNCTIONS_H
#define BUILDSCRIPT_UTILS_FUNCTIONS_H

#include <functional>

/**
 * @brief a @c std::function that accepts one argument typed @c T and returns boolean value.
 * @tparam T the type of an argument.
 */
template <typename T>
using Predicate = std::function<bool(T)>;

#endif // BUILDSCRIPT_UTILS_FUNCTIONS_H