/*
 * UTF8.cpp
 * - Encoding implementation of UTF-8.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

using namespace BuildScript;

class UTF8Encoding : public Encoding {
public:
    UTF8Encoding() : Encoding(u8"utf8") {}

    virtual int DecodeChar(const char* buffer, const char* end, int &length) const override {
        const unsigned char* buf = reinterpret_cast<const unsigned char*>(buffer);

        auto delta = end - buffer;

        if (delta < 1) {
            length = 0;
            return Encoding::InvalidEncoding;
        }

        // Check buffer starts with continuation charater.
        if ((*buf & 0xC0) == 0x80) {
            length = 1;
            return Encoding::InvalidEncoding;
        }

        int ch = 0;
        int count;

        // 0xxxxxxx
        if ((*buf & 0x80) == 0) {
            length = 1;
            return *buf;
        }
        // 110xxxxx 10xxxxxx
        else if ((*buf & 0xE0) == 0xC0) {
            count = 1;
            ch = (*buf++) & 0x1F;
        }
        // 1110xxxx 10xxxxxx 10xxxxxx
        else if ((*buf & 0xF0) == 0xE0) {
            count = 2;
            ch = (*buf++) & 0x0F;
        }
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else {
            count = 3;
            ch = (*buf++) & 0x07;
        }

        assert(count > 0 && count < 4);
        
        if (delta < count + 1) {
            length = delta;
            ch = Encoding::InvalidEncoding;
        }
        else {
            length = count + 1;

            do {
                ch = (ch << 6) | (*buf++ & 0x3F);
            } while (--count);

            if (ch < 0 || ch > 0x10FFFF)
                ch = Encoding::InvalidCharacter;
        }

        return ch;
    }

    // Check buffer starts with continuation character.
    inline bool CheckValidity(const char* buffer) const {
        return (*buffer & 0xC0) != 0x80;
    }

    virtual bool Convert(const char* buffer, size_t from, size_t to, std::string &out) const override {
        const char* begin = buffer + from;
        const char* end = buffer + to;

        if (!CheckValidity(begin) || !CheckValidity(end))
            return false;
        
        out = std::string(begin, end);
        return true;
    }
};

static UTF8Encoding utf8;