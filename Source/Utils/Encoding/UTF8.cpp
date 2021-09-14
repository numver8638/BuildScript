/*
 * UTF8.cpp
 * - Encode or decode character or string.
 *   This file implements UTF-8.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

#include <BuildScript/Utils/CharType.h>
#include <BuildScript/Utils/PointerArith.h>

#include "EncodingRegisterer.h"

using namespace BuildScript;

static inline bool IsContinuation(uint8_t ch) {
    return (ch & 0xC0) == 0x80;
}

static const uint8_t BOM[3] = { 0xEF, 0xBB, 0xBF };

class UTF8 : public Encoding {
public:
    explicit UTF8()
        : Encoding("utf-8") {}

    char32_t DecodeChar(const void* buf, const void* end, size_t& used) override {
        if (buf < end) {
            int length;
            const auto* buffer = static_cast<const uint8_t*>(buf);
            char32_t ch;

            // Get expected length of the character
            if (*buffer < 0x80) {
                // Fast end
                used = 1;
                return *buffer;
            }
            else if ((*buffer & 0xE0) == 0xC0) {
                ch = (*buffer & 0x1F);
                length = 2;
            }
            else if ((*buffer & 0xF0) == 0xE0) {
                ch = (*buffer & 0x0F);
                length = 3;
            }
            else if ((*buffer & 0xF8) == 0xF0) {
                ch = (*buffer & 0x07);
                length = 4;
            }
            else {
                // continuation character in current position
                // or invalid character.
                while (buffer < end && IsContinuation(*buffer)) {
                    ++buffer;
                }

                used = difference(buf, buffer);
                return InvalidEncoding;
            }

            // Modify end position to reduce comparison.
            end = buffer + length;

            // Move cursor to next because character is consumed in length check.
            ++buffer;

            // decode
            while (buffer < end && IsContinuation(*buffer)) {
                ch = (ch << 6) | (*buffer++ & 0x3F);
            }

            // Check decode is done successfully.
            if (buffer != end) {
                used = difference(buf, buffer);
                return InvalidEncoding;
            }
            else {
                used = length;

                // Overlong encoding check.
                if ((used == 2 && ch < 0x80) || (used == 3 && ch < 0x800) || (used == 4 && ch < 0x10000)) {
                    return InvalidEncoding;
                }

                // Range check
                return IsInvalidCharacter(ch) ? InvalidCharacter : ch;
            }
        }
        else {
            used = 0;
            return 0;
        }
    }

    bool EncodeChar(char32_t ch, void* buf, const void* end, size_t &length) override {
        if (buf < end) {
            if (IsInvalidCharacter(ch) || ch == InvalidCharacter || ch == InvalidEncoding) {
                length = 0;
                return false;
            }

            auto* buffer = static_cast<uint8_t*>(buf);

            if (ch < 0x80) {
                *(buffer + 0) = ch & 0x7F;
                length = 1;
            }
            else if (ch < 0x800 && difference(buffer, end) >= 2) {
                *(buffer + 0) = ((ch >>  6) & 0x1F) | 0xC0;
                *(buffer + 1) = ((ch >>  0) & 0x3F) | 0x80;
                length = 2;
            }
            else if (ch < 0x10000 && difference(buffer, end) >= 3) {
                *(buffer + 0) = ((ch >> 12) & 0x0F) | 0xE0;
                *(buffer + 1) = ((ch >>  6) & 0x3F) | 0x80;
                *(buffer + 2) = ((ch >>  0) & 0x3F) | 0x80;
                length = 3;
            }
            else if (difference(buffer, end) >= 4) {
                *(buffer + 0) = ((ch >> 18) & 0x07) | 0xF0;
                *(buffer + 1) = ((ch >> 12) & 0x3F) | 0x80;
                *(buffer + 2) = ((ch >>  6) & 0x3F) | 0x80;
                *(buffer + 3) = ((ch >>  0) & 0x3F) | 0x80;
                length = 4;
            }
            else {
                length = 0;
                return false;
            }

            return true;
        }
        else {
            length = 0;
            return false;
        }
    }

    size_t EncodeString(const std::string &string, void *buffer, const void *end) override {
        // Bypass decode-encode loop. This assumes input string always valid.
        assert(buffer <= end);

        if (string.length() < difference(buffer, end)) {
            std::memcpy(buffer, string.c_str(), string.length());
            return string.length();
        }
        else {
            // Find latest valid UTF-8 position
            auto it = string.crbegin();
            while (it != string.crend()) {
                if (!IsContinuation(*it)) {
                    --it; // Get before start character

                    if (-std::distance(string.crend(), it) < difference(buffer, end)) {
                        break;
                    }
                }
            }

            auto length = -std::distance(string.crend(), it);
            std::memcpy(buffer, string.c_str(), length);

            return length;
        }
    }

    std::string DecodeString(const void *buffer, const void *end, size_t &used) override {
        // Bypass decode-encode loop. This assumes input buffer always valid.

        used = difference(buffer, end);
        return std::string(static_cast<const char*>(buffer), difference(buffer, end));
    }

    size_t CheckPreamble(const void *buffer, const void *end) override {
        return (difference(end, buffer) > 3 && std::memcmp(buffer, BOM, 3) == 0) ? 3 : 0;
    }
}; // end class UTF8

REGISTER_ENCODING(UTF8)