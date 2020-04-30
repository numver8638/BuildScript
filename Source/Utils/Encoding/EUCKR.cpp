/*
 * EUCKR.cpp
 * - Encoding implementation of EUC-KR(CP949).
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

using namespace BuildScript;

extern const char16_t *EUCKRtoUTF8[]; // Defined at EUCKR.ConversionTable.cpp.

class EUCKREncoding : public Encoding {
public:
    EUCKREncoding() : Encoding(u8"euckr") {}

    virtual int DecodeChar(const char* buffer, const char* end, int &length) const override {
        if (end < buffer) {
            length = 0;
            return Encoding::InvalidEncoding;
        }
        
        unsigned char ch = *reinterpret_cast<const unsigned char*>(buffer);
        
        if (ch < 128) {
            length = 1;
            return ch;
        }
        else if (buffer + 1 > end) {
            length = 1;
            return Encoding::InvalidEncoding;
        }
        else {
            length = 2;
            unsigned char trailByte = *reinterpret_cast<const unsigned char*>(buffer + 1);

            if (EUCKRtoUTF8[ch] != nullptr)
                return EUCKRtoUTF8[ch][trailByte] == 0 ? Encoding::InvalidEncoding : EUCKRtoUTF8[ch][trailByte];
            else 
                return Encoding::InvalidEncoding;
        }
    }
};

static EUCKREncoding euckr;