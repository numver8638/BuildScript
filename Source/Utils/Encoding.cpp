/*
 * Encoding.cpp
 * - .
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

#include <cassert>

using namespace BuildScript;

static std::map<std::string, Encoding*>& GetMap() {
    static std::map<std::string, Encoding*> map;
    return map;
}

Encoding::Encoding(const std::string& name)
    : m_name(name) {
    // Self registering
    GetMap().emplace(name, this);
}

// static
Encoding* Encoding::GetEncoding(const std::string& encoding) {
    auto it = GetMap().find(encoding);

    return (it == GetMap().end()) ? nullptr : it->second;
}

// static
const std::vector<Encoding*> Encoding::GetEncodings() {
    std::vector<Encoding*> encodings;

    for (auto& e : GetMap())
        encodings.push_back(e.second);
    
    return encodings;
}

int Encoding::ToUTF8(int ch, char* buf) const {
    assert((0 <= ch) && (ch <= 0x10FFFF));
    assert((ch < 0xD800) && (0xDFFF < ch));

    if (ch <= 0x7F) {
        buf[0] = ch;
        return 1;
    }
    else if (ch <= 0x7FF) {
        buf[0] = (ch         & 0x3F) | 0x80;
        buf[1] = ((ch >>  6) & 0x1F) | 0xC0;
        return 2;
    }
    else if (ch <= 0xFFFF) {
        buf[0] = (ch         & 0x3F) | 0x80;
        buf[1] = ((ch >>  6) & 0x3F) | 0x80;
        buf[2] = ((ch >> 12) & 0x0F) | 0xE0;
        return 3;
    }
    else {
        buf[0] = (ch         & 0x3F) | 0x80;
        buf[1] = ((ch >>  6) & 0x3F) | 0x80;
        buf[2] = ((ch >> 12) & 0x3F) | 0x80;
        buf[2] = ((ch >> 18) & 0x07) | 0xF0;
        return 4;
    }
}

bool Encoding::Convert(const char* buffer, size_t from, size_t to, std::string &out) const {
    const char* current = buffer + from;
    const char* end = buffer + to;
    char utf8_buffer[4] = { 0, };

    while (current < end) {
        int decoded_length = 0;
        int ch = DecodeChar(current, end, decoded_length);
        current += decoded_length;

        if (ch == Encoding::InvalidCharacter || ch == Encoding::InvalidEncoding) {
            // Stop converting and return false.
            return false;
        }

        int encoded_length = ToUTF8(ch, utf8_buffer);
        out.append(utf8_buffer, utf8_buffer + encoded_length);
    }

    return true;
}