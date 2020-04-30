/*
 * UTF16.cpp
 * - Encoding implementation of UTF-16 LE and UTF-16 BE.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

#include <boost/endian.hpp>

using namespace BuildScript;
namespace endian = boost::endian;

class UTF16Encoding : public Encoding {
public:
    UTF16Encoding() : Encoding(u8"utf16") {}

    virtual int DecodeChar(const char* buffer, const char* end, int &length) const override {
        auto delta = end - buffer;
        
        if (delta < 2) {
            length = 0;
            return Encoding::InvalidEncoding;
        }
        
        char16_t high = endian::little_to_native(*reinterpret_cast<const char16_t*>(buffer));

        if ((0xD800 <= high) && (high <= 0xDBFF)) {
            if (delta < 4) {
                length = delta;
                return Encoding::InvalidEncoding;
            }
            
            length = 4;

            char16_t low = endian::little_to_native(*reinterpret_cast<const char16_t*>(buffer + 2));
            
            if ((low < 0xDC00) || (0xDFFF < low))
                // Not a low surrogate
                return Encoding::InvalidEncoding;

            return (((high & 0x3FF) + 1) << 10) | (low & 0x3FF);
        }
        else {
            length = 2;

            if ((0xDC00 <= high) && (high <= 0xDFFF))
                // Low surrogate
                return Encoding::InvalidEncoding;
            else
                return high;
        }
    }
}; // end class UTF16Encoding

class UTF16BEEncoding : public Encoding {
public:
    UTF16BEEncoding() : Encoding(u8"utf16be") {}

    virtual int DecodeChar(const char* buffer, const char* end, int &length) const override {
        auto delta = end - buffer;
        
        if (delta < 2) {
            length = 0;
            return Encoding::InvalidEncoding;
        }

        char16_t high = endian::big_to_native(*reinterpret_cast<const char16_t*>(buffer));

        if ((0xD800 <= high) && (high <= 0xDBFF)) {
            if (delta < 4) {
                length = delta;
                return Encoding::InvalidEncoding;
            }
            
            length = 4;

            char16_t low = endian::big_to_native(*reinterpret_cast<const char16_t*>(buffer + 2));
            
            if ((low < 0xDC00) || (0xDFFF < low))
                // Not a low surrogate
                return Encoding::InvalidEncoding;

            return (((high & 0x3FF) + 1) << 10) | (low & 0x3FF);
        }
        else {
            length = 2;

            if ((0xDC00 <= high) && (high <= 0xDFFF))
                // Low surrogate
                return Encoding::InvalidEncoding;
            else
                return high;
        }
    }
}; // end class UTF16BEEncoding

static UTF16Encoding utf16;
static UTF16BEEncoding utf16be;