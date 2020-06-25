/*
 * SourceText.h
 * - Represent a source code.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_SOURCETEXT_H
#define BUILDSCRIPT_COMPILER_SOURCETEXT_H

#include <functional>
#include <string>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/optional.hpp>

#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Utils/Encoding.h>
#include <BuildScript/Utils/NonCopyable.h>

namespace BuildScript {
    /**
     * @brief Represent a source code.
     */
    class EXPORT_API SourceText : NonCopyable {
    public:
        /** Source name when SourceText is constructed with buffer. */
        static constexpr const char* InternalSourceName = u8"<source>";

        enum ErrorCode : int {
            EndOfFile = -1,                                 //!< Cursor is reached at the end.
            InvalidEncoding = Encoding::InvalidEncoding,    //!< Text is not encoded correctly.
            InvalidCharacter = Encoding::InvalidCharacter,  //!< Decoded character is invalid unicode character.
        };

    private:
        boost::optional<boost::iostreams::mapped_file> m_file;
        std::string m_filename;
        Encoding& m_encoding;
        const int m_tabsize;

        const char* m_begin;
        const char* m_current;
        const char* m_end;

        unsigned m_line;
        unsigned m_column;

    public:
        /**
         * @brief Construct SourceText with file.
         * @param filename a file path to source.
         * @param encoding an encoding of file.
         * @param tabsize the column size of tab.
         * @throw std::ios_base::failure if failed to open file.
         */
        SourceText(const std::string& filename, Encoding& encoding = Encoding::UTF8(), int tabsize = 4);

        /**
         * @brief Construct SourceText with buffer.
         * @warning This assumes encoding of buffer is UTF-8.
         * @param begin start address of buffer.
         * @param end end address of buffer.
         * @param tabsize the column size of tab.
         */
        SourceText(const char* begin, const char* end, int tabsize = 4);

        /**
         * @brief Peek a character.
         * @return A unicode character or ErrorCode.
         * @see SourceText::ErrorCode
         */
        int PeekChar() const;

        /**
         * @brief Consume current character and advance cursor.
         */
        void Consume();

        /**
         * @brief Consume if current character is desired one.
         * @param desired a desired character.
         * @return true if consumed, otherwise false.
         */
        bool ConsumeIf(int desired) {
            if (PeekChar() == desired) {
                Consume();
                return true;
            } else {
                return false;
            }
        }

        /**
         * @brief Consume characters until given condition is satisfied.
         * @param cond the condition.
         * @return the position of after last consumed character.
         */
        SourcePosition ConsumeWhile(std::function<bool(int)> cond) {
            while (cond(PeekChar()) && PeekChar() != EndOfFile)
                Consume();

            return GetPosition();
        }

        /**
         * @brief Get the current cursor position in this object.
         * @return the current position.
         */
        SourcePosition GetPosition() const {
            return SourcePosition(static_cast<size_t>(m_current - m_begin), m_line, m_column);
        }

        /**
         * @brief Get the name of source text.
         * @return the name of file.
         */
        const std::string& GetFileName() const { return m_filename; }

        /**
         * @brief Get a partial text in this object. 
         * @param from the start position of text.
         * @param to the end position of text.
         * @return a string from the text.
         */
        std::string GetString(const SourcePosition& from, const SourcePosition& to) const {
            return GetString(from.Cursor, to.Cursor);
        }

        /**
         * @brief Get a partial text in this object. 
         * @param from the start position of text.
         * @param to the end position of text.
         * @return a string from the text.
         */
        std::string GetString(size_t from, size_t to) const {
            std::string ret;
            m_encoding.Convert(m_begin, from, to, ret);

            return ret;
        }
    }; // end class SourceText
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_SOURCETEXT_H