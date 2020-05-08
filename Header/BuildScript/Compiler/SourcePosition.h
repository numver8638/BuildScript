/*
 * SourcePosition.h
 * - Represent a position in the source text.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_SOURCEPOSITION_H
#define BUILDSCRIPT_COMPILER_SOURCEPOSITION_H

#include <cstddef>

namespace BuildScript {
    /**
     * @brief Represent a position in the source text.
     */
    struct SourcePosition {
        size_t Cursor;   //!< The cursor in the source text.
        unsigned Line;   //!< The line from the cursor.
        unsigned Column; //!< The column from the cursor.

        /**
         * @brief Construct empty object.
         */
        SourcePosition()
            : Cursor(0), Line(0), Column(0) {}

        /**
         * @brief Construct SourcePosition.
         * @param cursor the cursor.
         * @param line the line.
         * @param column the column.
         */
        SourcePosition(size_t cursor, unsigned line, unsigned column)
            : Cursor(cursor), Line(line), Column(column) {}

        /**
         * @brief Copy construct with other object.
         * @param rhs the SourcePosition object to copy.
         */
        SourcePosition(const SourcePosition& rhs) = default;

        /**
         * @brief Copy-assign other object.
         * @param rhs the SourcePosition object to assign.
         * @return the assigned object itself.
         */
        SourcePosition& operator =(const SourcePosition& rhs) = default;

        /**
         * @brief Test if data is valid.
         * @return true if valid, otherwise false.
         */
        bool IsValid() const {
            return Line != 0 && Column != 0;
        }

        /**
         * @see SourcePosition::IsValid() const
         */
        explicit operator bool() const {
            return IsValid();
        }
    }; // end struct SourcePosition
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_SOURCEPOSITION_H