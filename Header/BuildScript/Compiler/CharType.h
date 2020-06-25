/*
 * CharType.h
 * - Classify the character.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_CHARTYPE_H
#define BUILDSCRIPT_COMPILER_CHARTYPE_H

namespace BuildScript {
    /**
     * @brief Check character is in range of not allowed unicode characters as first character.
     * @param ch A unicode character.
     * @return true if character is not allowed, otherwise false.
     */
    static inline bool IsNotAllowedUnicodeInitially(int ch) {
        // C11 Annex D.2
        // Ranges of characters disallowed initially
        return  (0x0300 <= ch && ch <= 0x036F) ||
                (0x1DC0 <= ch && ch <= 0x1DFF) ||
                (0x20D0 <= ch && ch <= 0x20FF) ||
                (0xFE20 <= ch && ch <= 0xFE2F) ;
    }

    /**
     * @brief Check character is in range of allowed unicode characters.
     * @param ch A unicode character.
     * @return true if character is allowed, otherwise false.
     */
    static inline bool IsAllowedUnicode(int ch) {
        // C11 Annex D.1
        // Ranges of characters allowed
        return  ch == 0x00A8 ||
                ch == 0x00AA ||
                ch == 0x00AD ||
                ch == 0x00AF ||
                (0x00B2 <= ch && ch <= 0x00B5) ||
                (0x00B7 <= ch && ch <= 0x00BA) ||
                (0x00BC <= ch && ch <= 0x00BE) ||
                (0x00C0 <= ch && ch <= 0x00D6) ||
                (0x00D8 <= ch && ch <= 0x00F6) ||
                (0x00F8 <= ch && ch <= 0x00FF) ||
                (0x0100 <= ch && ch <= 0x167F) ||
                (0x1681 <= ch && ch <= 0x180D) ||
                (0x180F <= ch && ch <= 0x1FFF) ||
                (0x200B <= ch && ch <= 0x200D) ||
                (0x202A <= ch && ch <= 0x202E) ||
                (0x203F <= ch && ch <= 0x2040) ||
                ch == 0x2054 ||
                (0x2060 <= ch && ch <= 0x206F) ||
                (0x2070 <= ch && ch <= 0x218F) ||
                (0x2460 <= ch && ch <= 0x24FF) ||
                (0x2776 <= ch && ch <= 0x2793) ||
                (0x2C00 <= ch && ch <= 0x2DFF) ||
                (0x2E80 <= ch && ch <= 0x2FFF) ||
                (0x3004 <= ch && ch <= 0x3007) ||
                (0x3021 <= ch && ch <= 0x302F) ||
                (0x3031 <= ch && ch <= 0x303F) ||
                (0x3040 <= ch && ch <= 0xD7FF) ||
                (0xF900 <= ch && ch <= 0xFD3D) ||
                (0xFD40 <= ch && ch <= 0xFDCF) ||
                (0xFDF0 <= ch && ch <= 0xFE44) ||
                (0xFE47 <= ch && ch <= 0xFFFD) ||
                (0x10000 <= ch && ch <= 0x1FFFD) ||
                (0x20000 <= ch && ch <= 0x2FFFD) ||
                (0x30000 <= ch && ch <= 0x3FFFD) ||
                (0x40000 <= ch && ch <= 0x4FFFD) ||
                (0x50000 <= ch && ch <= 0x5FFFD) ||
                (0x60000 <= ch && ch <= 0x6FFFD) ||
                (0x70000 <= ch && ch <= 0x7FFFD) ||
                (0x80000 <= ch && ch <= 0x8FFFD) ||
                (0x90000 <= ch && ch <= 0x9FFFD) ||
                (0xA0000 <= ch && ch <= 0xAFFFD) ||
                (0xB0000 <= ch && ch <= 0xBFFFD) ||
                (0xC0000 <= ch && ch <= 0xCFFFD) ||
                (0xD0000 <= ch && ch <= 0xDFFFD) ||
                (0xE0000 <= ch && ch <= 0xEFFFD) ;
    }

    /**
     * @brief Check character is whitespace.
     * @param ch A unicode character.
     * @return true if character is whitespace, otherwise false.
     */
    static inline bool IsWhitespace(int ch) {
        return ch == u' ' || ch == u'\t';
    }

    /**
     * @brief Check character can be used in identifier.
     * @param ch A unicode character.
     * @return true if character can be used in identifier, otherwise false.
     */
    static inline bool IsIdentifier(int ch) {
        return ch == u'_' ||
            (u'a' <= ch && ch <= u'z') ||
            (u'A' <= ch && ch <= u'Z') ||
            (u'0' <= ch && ch <= u'9') ||
            IsAllowedUnicode(ch);
    }

    /**
     * @brief Check character is EOL(end of line).
     * @param ch A unicode character.
     * @return true if character is EOL, otherwise false.
     */
    static inline bool IsEOL(int ch) {
        return ch == u'\r' || ch == u'\n' || ch == -1/*SourceText::EndOfFile*/;
    }

    /**
     * @brief Check character is binary digit.
     * @param ch A unicode character.
     * @return true if character is binary digit, otherwise false.
     */
    static inline bool IsBinary(int ch) {
        return u'0' == ch || ch == u'1';
    }

    /**
     * @brief Check character is octal digit.
     * @param ch A unicode character.
     * @return true if character is octal digit, otherwise false.
     */
    static inline bool IsOctal(int ch) {
        return u'0' <= ch && ch <= u'7';
    }

    /**
     * @brief Check character is decimal digit.
     * @param ch A unicode character.
     * @return true if character is decimal digit, otherwise false.
     */
    static inline bool IsDecimal(int ch) {
        return u'0' <= ch && ch <= u'9';
    }

    /**
     * @brief Check character is hexadecimal digit.
     * @param ch A unicode character.
     * @return true if character is hexadecimal digit, otherwise false.
     */
    static inline bool IsHexadecimal(int ch) {
        return IsDecimal(ch) ||
            (u'a' <= ch && ch <= u'f') ||
            (u'A' <= ch && ch <= u'F');
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_CHARTYPE_H