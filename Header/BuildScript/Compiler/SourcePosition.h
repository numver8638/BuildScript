/*
 * SourcePosition.h
 * - Represents position in the source text.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_SOURCEPOSITION_H
#define BUILDSCRIPT_COMPILER_SOURCEPOSITION_H

#include <cassert>
#include <cstddef>

namespace BuildScript {
    /**
     * @brief Represents position in the source text.
     */
    struct SourcePosition {
        size_t Cursor = 0;   //!< The position in the source text.
        unsigned Line = 0;   //!< The line from the cursor.
        unsigned Column = 0; //!< The column from the cursor.

        SourcePosition() = default;

        /**
         * @brief Create @c SourcePosition.
         * @param cursor the cursor.
         * @param line the line.
         * @param column the column.
         */
        SourcePosition(size_t cursor, unsigned line, unsigned column)
            : Cursor(cursor), Line(line), Column(column) {}

        SourcePosition(const SourcePosition& rhs) = default;

        SourcePosition& operator =(const SourcePosition& rhs) = default;

        bool operator <(const SourcePosition& rhs) const {
            return IsValid() && rhs.IsValid() && (Cursor < rhs.Cursor);
        }

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

        /**
         * @brief Get an empty @c SourcePosition.
         * @return An empty @c SourcePosition.
         */
        static const SourcePosition& Empty() {
            static SourcePosition EMPTY;

            return EMPTY;
        }
    }; // end struct SourcePosition

    /**
     * @brief Represents left-closed range in the source text.
     */
    struct SourceRange {
        SourcePosition Begin;   //!< Beginning position of the range.
        SourcePosition End;     //!< Ending position of the range.

        SourceRange() = default;

        /**
         * @brief Construct the object with one position.
         * @param pos a position.
         */
        explicit SourceRange(const SourcePosition& pos)
            : Begin(pos), End(pos) {}

        /**
         * @brief Construct the object with begin and end position.
         * @param begin a start position.
         * @param end a end position.
         */
        SourceRange(const SourcePosition& begin, const SourcePosition& end)
            : Begin(begin), End(end) {
            assert((begin.Cursor <= end.Cursor) && "begin > end");
        }

        SourceRange(const SourceRange& rhs) = default;

        SourceRange& operator =(const SourceRange& rhs) = default;

        /**
         * @brief Test that data is valid.
         * @return true if valid, otherwise false.
         */
        bool IsValid() const {
            return Begin.IsValid() && End.IsValid();
        }

        /**
         * @see SourceRange::IsValid()
         */
        explicit operator bool() const {
            return IsValid();
        }

        /**
         * @brief Merge two ranges into one range.
         * @param begin front range to be merged.
         * @param end rear range to be merged.
         * @return a merged @c SourceRange.
         */
        static inline SourceRange Merge(const SourceRange& begin, const SourceRange& end) {
            return { begin.Begin, end.End };
        }

        /**
         * @brief Merge a position and a range into one range.
         * @param begin front position to be merged.
         * @param end rear range to be merged.
         * @return a merged @c SourceRange.
         */
        static inline SourceRange Merge(const SourcePosition& begin, const SourceRange& end) {
            return { begin, end.End };
        }

        /**
         * @brief Merge a position and a range into one range.
         * @param begin front range to be merged.
         * @param end rear position to be merged.
         * @return a merged @c SourceRange.
         */
        static inline SourceRange Merge(const SourceRange& begin, const SourcePosition& end) {
            return { begin.Begin, end };
        }
    }; // end struct SourceRange
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_SOURCEPOSITION_H