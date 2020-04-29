/*
 * TypeTraits.h
 * - Type traits used in BuildScript.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_TYPETRAITS_H
#define BUILDSCRIPT_UTILS_TYPETRAITS_H

#include <type_traits>

/**
 * @brief Query and enable if derive type is inherited base type.
 * @tparam Base The base type.
 * @tparam Derive The type derived from base type.
 */
template <typename Base, typename Derive>
using is_base_of_t = typename std::enable_if<std::is_base_of<Base, Derive>::value, Derive>::type;

#endif // BUILDSCRIPT_UTILS_TYPETRAITS_H