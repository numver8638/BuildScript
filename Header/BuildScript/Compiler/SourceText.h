/*
 * SourceText.h
 * - Represents the source text.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_SOURCETEXT_H
#define BUILDSCRIPT_COMPILER_SOURCETEXT_H

#include <cassert>

#include <BuildScript/Config.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Utils/Encoding.h>
#include <BuildScript/Utils/Functions.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    /**
     * @brief Represents the source text.
     */
    class EXPORT_API SourceText final : NonCopyable {
    public:
        /**
         * @brief Represents end of file(EOF).
         */
        static constexpr auto EndOfFile = static_cast<char32_t>(-1);

        /**
         * @copybrief BuildScript::Encoding::InvalidEncoding
         */
        static constexpr auto InvalidEncoding = Encoding::InvalidEncoding;

        /**
         * @copybrief BuildScript::Encoding::InvalidCharacter
         */
        static constexpr auto InvalidCharacter =  Encoding::InvalidCharacter;

    private:
        std::string m_name;

        const char* m_begin;
        const char* m_current;
        const char* m_next;
        const char* m_end;

        Encoding& m_encoding;

        const unsigned m_tabsize;

        unsigned m_line = 1;
        unsigned m_column = 0;

        char32_t m_ch = EndOfFile;

    public:
        /**
         * @brief Create @c SourceText from given buffer and encoding.
         * @param name name of this source text. Used in error reporting.
         * @param buffer the text buffer.
         * @param length the length of the buffer.
         * @param encoding reference of @c Encoding to decode text.
         * @param tabsize size of tab applied to tab character('\\t').
         */
        SourceText(std::string name, const char* buffer, size_t length, Encoding& encoding, unsigned tabsize = 4);

        /**
         * @brief Create @c SourceText from parent with range.
         * @param parent reference of @c SourceText.
         * @param range range of the text from parent.
         */
        SourceText(const SourceText& parent, const SourceRange& range);

        /**
         * @brief Peek a character.
         * @return a character.
         */
        char32_t PeekChar() const { return m_ch; }

        /**
         * @brief Consume a character.
         */
        void ConsumeChar();

        /**
         * @brief Peek and consume character.
         * @return a character.
         */
        char32_t AdvanceChar() {
            auto ch = PeekChar();

            ConsumeChar();

            return ch;
        }

        /**
         * @brief Consume a character if the character is desired one.
         * @param desired desired character.
         * @return @c true if the character has consumed otherwise returns @c false.
         */
        bool ConsumeIf(char32_t desired) {
            if (PeekChar() == desired) {
                ConsumeChar();
                return true;
            }
            else {
                return false;
            }
        }

        /**
         * @brief Consume characters while @c predicate returns @c true.
         * @param predicate a @c Predicate that determines to consume character.
         * @return next position of last consumed character.
         */
        SourcePosition ConsumeWhile(const Predicate<char32_t>& predicate) {
            auto ch = PeekChar();

            while (predicate(ch) && (ch != EndOfFile)) {
                ConsumeChar();
                ch = PeekChar();
            }

            return GetPosition();
        }

        /**
         * @brief Consume characters until @c predicate returns @c true.
         * @param predicate a @c Predicate that determines to consume character.
         * @return next position of last consumed character.
         */
        SourcePosition ConsumeUntil(const Predicate<char32_t>& predicate) {
            auto ch = PeekChar();

            while (!predicate(ch) && (ch != EndOfFile)) {
                ConsumeChar();
                ch = PeekChar();
            }

            return GetPosition();
        }

        /**
         * @brief Get a string from given range in the source.
         * @param range the range of the string.
         * @return partial text from the source text.
         */
        std::string GetString(const SourceRange& range) const { return GetString(range.Begin, range.End); }

        /**
         * @brief Get a string from given range in the source.
         * @param begin the start position of the range.
         * @param end the end position of the range.
         * @return partial text from the source text.
         */
        std::string GetString(const SourcePosition& begin, const SourcePosition& end) const;

        /**
         * @brief Get a current position indicated by cursor.
         * @return a current cursor position.
         */
        SourcePosition GetPosition() const { return { size_t(m_current - m_begin), m_line, m_column }; }

        /**
         * @brief Get the name of the source text.
         * @return the name of the source text.
         */
        const std::string& GetName() const { return m_name; }

        /**
         * @brief Get the name of the encoding.
         * @return the name of the encoding.
         */
        const std::string& GetEncodingName() const { return m_encoding.GetName(); }
    }; // end class SourceText
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_SOURCETEXT_H