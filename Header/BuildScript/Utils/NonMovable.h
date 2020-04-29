/*
 * NonMovable.h
 * - Make object non-movable.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_NONMOVABLE_H
#define BUILDSCRIPT_UTILS_NONMOVABLE_H

namespace BuildScript {
    /**
     * @brief Make object non-movable.
     */
    class NonMovable {
    protected:
        NonMovable() {}
        ~NonMovable() {}

        NonMovable(NonMovable&&) = delete;
        const NonMovable& operator =(NonMovable&&) = delete;
    }; // end class NonMovable
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_NONMOVABLE_H