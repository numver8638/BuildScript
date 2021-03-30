/*
 * AllStatic.h
 * - Disable creating object.
 * 
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_ALLSTATIC_H
#define BUILDSCRIPT_UTILS_ALLSTATIC_H

namespace BuildScript {
    /**
     * @brief Disable creating object.
     */
    class AllStatic {
    public:
        AllStatic() = delete;

        AllStatic(const AllStatic&) = delete;

        AllStatic(AllStatic&&) = delete;

        AllStatic& operator =(const AllStatic&) = delete;

        AllStatic& operator =(AllStatic&&) = delete;
    }; // end class AllStatic
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_ALLSTATIC_H