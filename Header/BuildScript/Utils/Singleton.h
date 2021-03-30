/*
 * Singleton.h
 * - Make object as singleton.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_SINGLETON_H
#define BUILDSCRIPT_UTILS_SINGLETON_H

#include <BuildScript/Utils/NonCopyable.h>
#include <BuildScript/Utils/NonMovable.h>

namespace BuildScript {
    /**
     * @brief Make object as singleton.
     * @tparam T The type to make singleton.
     * @see GetInstance()
     */
    template <typename T>
    class Singleton : NonCopyable, NonMovable {
    public:
        using Type = T;
    }; // end class Singleton

    /**
     * @brief Get instance of singleton object.
     * @warning This function is not thread-safe.
     * @tparam T The type of singleton object.
     * @return Reference of singleton object.
     */
    template <typename T>
    typename Singleton<T>::Type& GetInstance() {
        static T t;
        return t;
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_SINGLETON_H