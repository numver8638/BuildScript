/*
 * EUCKR.cpp
 * - Encode or decode character or string.
 *   This file implements encoding EUC-KR(CP949).
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

#include <BuildScript/Utils/PointerArith.h>

#include "EUCKR.ConversionTable.h"
#include "EncodingRegisterer.h"

using namespace BuildScript;

constexpr auto ONE_BYTE = 1;
constexpr auto TWO_BYTES = 2;

class EUCKR : public Encoding {
public:
    EUCKR()
        : Encoding("euc-kr") {}

    char32_t DecodeChar(const void* buf, const void* end, size_t& used) override {
        if (buf < end) {
            const auto* buffer = static_cast<const uint8_t*>(buf);

            if (*buffer < 0x80) {
                used = ONE_BYTE;
                return char32_t(*buffer);
            }
            else if (difference(buf, end) < 2 || EUCKRToUnicode[*buffer] == nullptr) {
                used = ONE_BYTE;
                return Encoding::InvalidEncoding;
            }
            else {
                used = TWO_BYTES;
                auto ch = EUCKRToUnicode[*buffer][*(buffer + 1)];

                return (ch == 0) ? Encoding::InvalidEncoding : ch;
            }
        }
        else {
            used = 0;
            return 0;
        }
    }

    bool EncodeChar(char32_t ch, void* buf, const void* end, size_t &length) override {
        auto* buffer = reinterpret_cast<uint8_t*>(buf);

        if (ch < 0x80 && buf < end) {
            *buffer = static_cast<uint8_t>(ch);
            length = ONE_BYTE;
            return true;
        }
        else if (difference(buf, end) < 2 || ch > 0xFFFF || UnicodeToEUCKR[ch] == 0) {
            length = 0;
            return false;
        }
        else {
            *(buffer + 0) = (UnicodeToEUCKR[ch] >> 8) & 0xFF;
            *(buffer + 1) = UnicodeToEUCKR[ch] & 0xFF;

            length = TWO_BYTES;
            return true;
        }
    }
}; // end class EUCKR

REGISTER_ENCODING(EUCKR)