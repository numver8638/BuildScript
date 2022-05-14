/*
 * Trilean.h
 * - Represents three-valued logic value.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_TRILEAN_H
#define BUILDSCRIPT_UTILS_TRILEAN_H

namespace BuildScript {
    /**
     * @brief Represents three-valued logic value.
     *
     * @ref https://nlp.stanford.edu/nlp/javadoc/javanlp/edu/stanford/nlp/util/Trilean.html
     * @ref https://en.wikipedia.org/wiki/Three-valued_logic
     */
    enum class Trilean {
        Unknown,
        True,
        False
    }; // end enum Trilean

    /**
     * @brief Test given value is not unknown.
     * @param value a @c Trilean value.
     * @return @c true if given value is not unknown otherwise @c false.
     */
    inline bool IsKnown(Trilean value) { return value != Trilean::Unknown; }

    /**
     * @brief Test given value is unknown.
     * @param value a @c Trilean value.
     * @return @c true if given value is unknown otherwise @c false.
     */
    inline bool IsUnknown(Trilean value) { return value == Trilean::Unknown; }

    /**
     * @brief Convert trilean to boolean.
     * @param value a @c Trilean value.
     * @return converted boolean value.
     */
    inline bool ToBoolean(Trilean value) {
        return (value == Trilean::True);
    }

    /**
     * @brief Convert boolean to trilean.
     * @param value a boolean value.
     * @return converted @c Trilean value.
     */
    inline Trilean ToTrilean(bool value) {
        return value ? Trilean::True : Trilean::False;
    }

    inline Trilean operator &(Trilean left, Trilean right) {
        if (IsUnknown(left) || IsUnknown(right)) {
            return Trilean::Unknown;
        }

        return ToTrilean(ToBoolean(left) & ToBoolean(right));
    }

    inline Trilean operator |(Trilean left, Trilean right) {
        if (IsUnknown(left) || IsUnknown(right)) {
            return Trilean::Unknown;
        }

        return ToTrilean(ToBoolean(left) | ToBoolean(right));
    }

    inline Trilean operator ^(Trilean left, Trilean right) {
        if (IsUnknown(left) || IsUnknown(right)) {
            return Trilean::Unknown;
        }

        return ToTrilean(ToBoolean(left) ^ ToBoolean(right));
    }

    inline Trilean operator !(Trilean value) {
        return IsUnknown(value) ? Trilean::Unknown : ToTrilean(!ToBoolean(value));
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_TRILEAN_H