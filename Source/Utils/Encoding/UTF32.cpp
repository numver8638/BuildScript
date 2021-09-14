/*
 * UTF32.cpp
 * - Encode or decode character or string.
 *   This file implements UTF-32.
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

constexpr auto ONE_CHAR = sizeof(char32_t);

static const uint8_t UTF32LE_BOM[4] = { 0xFF, 0xFE, 0x00, 0x00 };
static const uint8_t UTF32BE_BOM[4] = { 0x00, 0x00, 0xFE, 0xFF };

class UTF32LE : public Encoding {
public:
    UTF32LE()
        : Encoding("utf-32le") {}

    char32_t DecodeChar(const void* buffer, const void* end, size_t& used) override {
        if (difference(buffer, end) >= 4) {
            used = ONE_CHAR;
            auto ch = endian::little_to_native(*reinterpret_cast<const char32_t*>(buffer));
            return IsInvalidCharacter(ch) ? InvalidCharacter : ch;
        }
        else {
            used = 0;
            return 0;
        }
    }

    bool EncodeChar(char32_t ch, void* buffer, const void* end, size_t &length) override {
        if (difference(buffer, end) >= 4 && !IsInvalidCharacter(ch)) {
            auto* casted_buffer = reinterpret_cast<char32_t*>(buffer);

            *casted_buffer = endian::native_to_little(ch);
            length = ONE_CHAR;
            return true;
        }
        else {
            length = 0;
            return false;
        }
    }

    size_t CheckPreamble(const void *buffer, const void *end) override {
        return difference(buffer, end) >= 4 && std::memcmp(buffer, UTF32LE_BOM, sizeof(UTF32LE_BOM)) == 0 ? 4 : 0;
    }
}; // end class UTF32LE

class UTF32BE : public Encoding {
public:
    UTF32BE()
        : Encoding("utf-32be") {}

    char32_t DecodeChar(const void* buffer, const void* end, size_t& used) override {
        if (difference(buffer, end) >= 4) {
            used = ONE_CHAR;
            auto ch = endian::big_to_native(*reinterpret_cast<const char32_t*>(buffer));
            return IsInvalidCharacter(ch) ? InvalidCharacter : ch;
        }
        else {
            used = 0;
            return 0;
        }
    }

    bool EncodeChar(char32_t ch, void* buffer, const void* end, size_t &length) override {
        if (difference(buffer, end) >= 4  && !IsInvalidCharacter(ch)) {
            auto* casted_buffer = reinterpret_cast<char32_t*>(buffer);

            *casted_buffer = endian::native_to_big(ch);

            length = ONE_CHAR;
            return true;
        }
        else {
            length = 0;
            return false;
        }
    }

    size_t CheckPreamble(const void *buffer, const void *end) override {
        return difference(buffer, end) >= 4 && std::memcmp(buffer, UTF32BE_BOM, sizeof(UTF32BE_BOM)) == 0 ? 4 : 0;
    }
}; // end class UTF32BE

REGISTER_ENCODING(UTF32LE)
REGISTER_ENCODING(UTF32BE)