/*
 * PointerArith.h
 * - Pointer arithmetic on void*
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_POINTERARITH_H
#define BUILDSCRIPT_UTILS_POINTERARITH_H

#include <cstddef>

/**
 * @brief Get difference between two pointers.
 * @param left some pointer.
 * @param right another pointer.
 * @return difference from @c right to @c left.
 */
inline std::ptrdiff_t difference(const void* left, const void* right) {
    return reinterpret_cast<const uint8_t*>(right) - reinterpret_cast<const uint8_t*>(left);
}

/**
 * @brief Add offset on pointer.
 * @param ptr the pointer.
 * @param offset the offset.
 * @return pointer that is added offset @c offset to @c ptr.
 */
inline void* add(void* ptr, std::ptrdiff_t offset) {
    return reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(ptr) + offset);
}

/**
 * @copydoc BuildScript::add(void*, std::ptrdiff_t)
 */
inline const void* add(const void* ptr, std::ptrdiff_t offset) {
    return reinterpret_cast<const void*>(reinterpret_cast<const uint8_t*>(ptr) + offset);
}

/**
 * @brief Subtract offset on pointer.
 * @param ptr the pointer
 * @param offset the offset.
 * @return pointer that is subtracted offset @c offset from @c ptr.
 */
inline void* sub(void* ptr, std::ptrdiff_t offset) {
    return reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(ptr) - offset);
}

#endif // BUILDSCRIPT_UTILS_POINTERARITH_H