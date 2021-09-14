/*
 * UTF16.cpp
 * - Encode or decode character or string.
 *   This file implements UTF-16.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

#include <boost/endian.hpp>

#include <BuildScript/Utils/CharType.h>
#include <BuildScript/Utils/PointerArith.h>

#include "EncodingRegisterer.h"

using namespace BuildScript;

namespace endian = boost::endian;

constexpr auto ONE_CHAR = sizeof(char16_t);
constexpr auto TWO_CHARS = sizeof(char16_t) * 2;

constexpr bool IsHighSurrogate(char16_t ch) {
    return (0xD800 <= ch) && (ch <= 0xDBFF);
}

constexpr bool IsLowSurrogate(char16_t ch) {
    return (0xDC00 <= ch) && (ch <= 0xDFFF);
}

constexpr uint16_t HIGH_SURROGATE_MASK  = 0x03FF;
constexpr uint16_t LOW_SURROGATE_MASK   = 0x03FF;
constexpr auto HIGH_SURROGATE_SHIFT     = 10;

static const uint8_t UTF16LE_BOM[2] = { 0xFF, 0xFE };
static const uint8_t UTF16BE_BOM[2] = { 0xFE, 0xFF };

class UTF16LE : public Encoding {
public:
    UTF16LE()
        : Encoding("utf-16le") {}

    char32_t DecodeChar(const void* buf, const void* end, size_t& used) override {
        auto length = difference(buf, end);

        if (length >= 2) {
            auto* buffer = static_cast<const char16_t*>(buf);

            auto high = endian::little_to_native<char16_t>(*buffer);

            if (IsHighSurrogate(high) && length >= 4) {
                auto low = endian::little_to_native<char16_t>(*(buffer + 1));

                if (!IsLowSurrogate(low)) {
                    // Not low surrogate after high surrogate.
                    used = ONE_CHAR;
                    return Encoding::InvalidEncoding;
                }
                else {
                    used = TWO_CHARS;
                    char32_t ch = ((high & HIGH_SURROGATE_MASK) + 0x40) << HIGH_SURROGATE_SHIFT | low & LOW_SURROGATE_MASK;

                    return IsInvalidCharacter(ch) ? Encoding::InvalidCharacter : ch;
                }
            }
            else if (IsLowSurrogate(high) || (IsHighSurrogate(high) && length < 4)) {
                used = ONE_CHAR;
                return Encoding::InvalidEncoding;
            }
            else {
                used = ONE_CHAR;
                return high;
            }
        }
        else {
            used = 0;
            return 0;
        }
    }

    bool EncodeChar(char32_t ch, void* buf, const void* end, size_t &length) override {
        auto size = difference(buf, end);

        if (size >= 2 && !IsInvalidCharacter(ch)) {
            auto* buffer = static_cast<char16_t *>(buf);

            if (ch < 0x10000) {
                *buffer = endian::native_to_little(char16_t(ch));
                length = ONE_CHAR;
            }
            else if (size >= 4) {
                *(buffer + 0) = endian::native_to_little(char16_t(0xD800 | ((ch >> HIGH_SURROGATE_SHIFT) & HIGH_SURROGATE_MASK) - 0x40));
                *(buffer + 1) = endian::native_to_little(char16_t(0xDC00 | (ch & LOW_SURROGATE_MASK)));

                length = TWO_CHARS;
            }
            else {
                // buffer too small.
                length = 0;
                return false;
            }

            return true;
        }
        else {
            // buffer too small or invalid.
            length = 0;
            return false;
        }
    }

    size_t CheckPreamble(const void *buffer, const void *end) override {
        return difference(buffer, end) >= 2 && std::memcmp(buffer, UTF16LE_BOM, sizeof(UTF16LE_BOM)) == 0 ? 2 : 0;
    }
}; // end class UTF16LE

class UTF16BE : public Encoding {
public:
    UTF16BE()
        : Encoding("utf-16be") {}

    char32_t DecodeChar(const void* buf, const void* end, size_t& used) override {
        auto length = difference(buf, end);

        if (length >= 2) {
            auto* buffer = static_cast<const char16_t*>(buf);

            auto high = endian::big_to_native<char16_t>(*buffer);

            if (IsHighSurrogate(high) && length >= 4) {
                auto low = endian::big_to_native<char16_t>(*(buffer + 1));

                if (!IsLowSurrogate(low)) {
                    // Not low surrogate after high surrogate.
                    used = ONE_CHAR;
                    return Encoding::InvalidEncoding;
                }
                else {
                    used = TWO_CHARS;
                    char32_t ch = ((high & HIGH_SURROGATE_MASK) + 0x40) << HIGH_SURROGATE_SHIFT | low & LOW_SURROGATE_MASK;

                    return IsInvalidCharacter(ch) ? Encoding::InvalidCharacter : ch;
                }
            }
            else if (IsLowSurrogate(high) || (IsHighSurrogate(high) && length < 4)) {
                used = ONE_CHAR;
                return Encoding::InvalidEncoding;
            }
            else {
                used = ONE_CHAR;
                return high;
            }
        }
        else {
            used = 0;
            return 0;
        }
    }

    bool EncodeChar(char32_t ch, void* buf, const void* end, size_t &length) override {
        auto size = difference(buf, end);

        if (size >= 2 && !IsInvalidCharacter(ch)) {
            auto* buffer = static_cast<char16_t*>(buf);

            if (ch < 0x10000) {
                *buffer = endian::native_to_big(char16_t(ch));
                length = ONE_CHAR;
            }
            else if (size >= 4) {
                *(buffer + 0) = endian::native_to_big(char16_t(0xD800 | ((ch >> HIGH_SURROGATE_SHIFT) & HIGH_SURROGATE_MASK) - 0x40));
                *(buffer + 1) = endian::native_to_big(char16_t(0xDC00 | (ch & LOW_SURROGATE_MASK)));

                length = TWO_CHARS;
            }
            else {
                // buffer too small.
                length = 0;
                return false;
            }

            return true;
        }
        else {
            // buffer too small or invalid.
            length = 0;
            return false;
        }
    }

    size_t CheckPreamble(const void *buffer, const void *end) override {
        return difference(buffer, end) >= 2 && std::memcmp(buffer, UTF16BE_BOM, sizeof(UTF16BE_BOM)) == 0 ? 2 : 0;
    }
}; // end class UTF16BE

REGISTER_ENCODING(UTF16LE)
REGISTER_ENCODING(UTF16BE)