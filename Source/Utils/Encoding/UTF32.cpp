/*
 * UTF32.cpp
 * - Encoding implementation of UTF-32 LE and UTF-32 BE.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

#include <boost/endian.hpp>

using namespace BuildScript;
namespace endian = boost::endian;

class UTF32Encoding : public Encoding {
public:
    UTF32Encoding() : Encoding(u8"utf32") {}

    virtual int DecodeChar(const char* buffer, const char* end, int &length) const override {
        if ((end - buffer) < 4) {
            length = end - buffer;
            return Encoding::InvalidEncoding;
        }
        
        length = 4;
        auto ch = endian::little_to_native(*reinterpret_cast<const int*>(buffer));

        if (ch < 0 || ch > 0x10FFFF)
            ch = Encoding::InvalidCharacter;
        
        return ch;
    }
}; // end class UTF32Encoding

class UTF32BEEncoding : public Encoding {
public:
    UTF32BEEncoding() : Encoding(u8"utf32be") {}

    virtual int DecodeChar(const char* buffer, const char* end, int &length) const override {
        if ((end - buffer) < 4) {
            length = end - buffer;
            return Encoding::InvalidEncoding;
        }
        
        length = 4;
        auto ch = endian::big_to_native(*reinterpret_cast<const int*>(buffer));

        if (ch < 0 || ch > 0x10FFFF)
            ch = Encoding::InvalidCharacter;
        
        return ch;
    }  
}; // end class UTF32BEEncoding

static UTF32Encoding utf32;
static UTF32BEEncoding utf32be;