/*
 * StringRef.h
 * - Reference of string in source text.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_STRINGREF_H
#define BUILDSCRIPT_COMPILER_STRINGREF_H

#include <cassert>
#include <string>

#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Compiler/SourceText.h>

namespace BuildScript {
    /**
     * @brief Reference of string in source text.
     */
    class StringRef {
    private:
        SourceText* m_text;
        size_t m_begin, m_end;

        StringRef(SourceText* source, size_t begin, size_t end)
            : m_text(source), m_begin(begin), m_end(end) {}

    public:
        StringRef()
            : StringRef(nullptr, 0, 0) {}
        
        StringRef(SourceText& source, const SourceRange& range)
            : StringRef(&source, range.Begin.Cursor, range.End.Cursor) {}

        StringRef(SourceText& source, const SourcePosition& begin, const SourcePosition& end)
            : StringRef(&source, begin.Cursor, end.Cursor) {}

        StringRef(const StringRef& rhs)
            : StringRef(rhs.m_text, rhs.m_begin, rhs.m_end) {}

        StringRef(StringRef&& rhs) noexcept
        : StringRef(rhs.m_text, rhs.m_begin, rhs.m_end) {
            // invalidate rhs
            rhs.m_text = nullptr;
            rhs.m_begin = 0;
            rhs.m_end = 0;
        }

        StringRef& operator =(const StringRef& rhs) = default;

        StringRef& operator =(StringRef&& rhs) noexcept {
            m_text = rhs.m_text;
            m_begin = rhs.m_begin;
            m_end = rhs.m_end;

            // invalidate rhs
            rhs.m_text = nullptr;
            rhs.m_begin = 0;
            rhs.m_end = 0;

            return *this;
        }

        bool operator ==(const StringRef& rhs) const {
            return ToString() == rhs.ToString();
        }

        bool operator !=(const StringRef& rhs) const {
            return ToString() != rhs.ToString();
        }

        bool IsEmpty() const { return m_begin == m_end; }

        bool IsValid() const { return m_text != nullptr; }

        explicit operator bool() const { return IsValid(); }

        std::string ToString() const {
            assert(IsValid());
            return m_text->GetString(m_begin, m_end);
        }

        // Allow implicit cast to std::string
        operator std::string() const { return ToString(); }
    }; // end class StringRef
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_STRINGREF_H