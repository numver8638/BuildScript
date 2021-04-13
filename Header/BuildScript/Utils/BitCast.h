/*
 * BitCast.h
 * - Provide safer way to cast different type bit by bit.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_BITCAST_H
#define BUILDSCRIPT_UTILS_BITCAST_H

#include <cstring>

/**
 * @brief Provide safer way to cast different type bit by bit.
 * @tparam Dest the type of cast to.
 * @tparam Source the type of cast from.
 * @param source the target to cast.
 * @return Casted object from @e Source to @e Dest.
 */
template <typename Dest, typename Source>
inline Dest bit_cast(const Source& source) {
    static_assert(sizeof(Dest) == sizeof(Source), "sizeof(Dest) != sizeof(Source)");
    
    Dest dest;
    std::memcpy(&dest, &source, sizeof(dest));
    return dest;
}

#endif // BUILDSCRIPT_UTILS_BITCAST_H