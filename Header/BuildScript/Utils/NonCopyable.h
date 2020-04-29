/*
 * NonCopyable.h
 * - Make object non-copyable.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_NONCOPYABLE_H
#define BUILDSCRIPT_UTILS_NONCOPYABLE_H

namespace BuildScript {
    /**
     * @brief Make object non-copyable.
     */
    class NonCopyable {
    protected:
        NonCopyable() {}
        ~NonCopyable() {}

        NonCopyable(const NonCopyable&) = delete;
        const NonCopyable& operator =(const NonCopyable&) = delete;
    }; // end class NonCopyable
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_NONCOPYABLE_H