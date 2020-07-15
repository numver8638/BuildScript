/*
 * StringRef.h
 * - A reference of string in the source text.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_STRINGREF_H
#define BUILDSCRIPT_COMPILER_STRINGREF_H

#include <string>

#include <BuildScript/Compiler/SourcePosition.h>

namespace BuildScript {
    class SourceText; // Defined in <BuildScript/Compiler/SourceText.h>
    /**
     * @brief A reference of string in the source text.
     */
    class EXPORT_API StringRef {
    private:
        SourceText* m_text;
        size_t m_begin, m_end;

        StringRef(SourceText* source, size_t begin, size_t end)
            : m_text(source), m_begin(begin), m_end(end) {}

    public:
        /**
         * @brief Construct empty object.
         */
        StringRef()
            : StringRef(nullptr, 0, 0) {}

        /**
         * @brief .
         * @param begin .
         * @param end .
         */
        StringRef(SourceText& source, const SourcePosition& begin, const SourcePosition& end)
            : StringRef(&source, begin.Cursor, end.Cursor) {}

        /**
         * @brief Construct from other object.
         * @param rhs other object.
         */
        StringRef(const StringRef& rhs) = default;

        /**
         * @brief Assign from other object.
         * @param rhs other object to assign.
         * @return the object itself.
         */
        StringRef& operator =(const StringRef& rhs) = default;

        /**
         * @brief .
         * @param rhs .
         * @return .
         */
        bool operator ==(const StringRef& rhs) const {
            return ToString() == rhs.ToString();
        }

        /**
         * @brief .
         * @param rhs .
         * @return .
         */
        bool operator !=(const StringRef& rhs) const {
            return ToString() != rhs.ToString();
        }

        /**
         * @brief Test if the length of string referenced by the object is zero.
         * @return true if the length is zero, otherwise false.
         * @warning This not indicates that the object is valid. To test validity of the object, use IsValid() instead.
         */
        bool IsEmpty() const { return m_begin == m_end; }

        /**
         * @brief Test if the object is valid.
         * @return true if the object is valid, otherwise false.
         */
        bool IsValid() const { return m_text != nullptr; }

        /**
         * @brief Test if the object is valid.
         * @return true if the object is valid, otherwise false.
         * @see StringRef::IsValid() const;
         */
        explicit operator bool() const { return IsValid(); }

        /**
         * @brief Return a string referenced by the object.
         * @return a string.
         * @warning Must be used in IsValid() is true.
         */
        std::string ToString() const;

        /**
         * @brief Implicit cast to std::string.
         * @return a string.
         * @see StringRef::ToString() const
         */
        operator std::string() const { return ToString(); }

        static StringRef Merge(const StringRef& left, const StringRef& right) {
            assert(left.m_text == right.m_text);

            return StringRef(left.m_text, left.m_begin, right.m_end);
        }
    }; // end class StringRef
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_STRINGREF_H