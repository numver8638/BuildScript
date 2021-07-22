/*
 * Encoding.cpp
 * - Encode or decode character or string.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Encoding.h>

#include <map>
#include <sstream>
#include <BuildScript/Utils/PointerArith.h>

using namespace BuildScript;

using EncodingMap = std::map<std::string, Encoding*>;

static EncodingMap& GetEncodingMap() {
    static EncodingMap map;

    return map;
}

std::string Encoding::DecodeString(const void* buffer, const void* end, size_t& used) {
    std::stringstream stream;
    char tempBuffer[8];
    size_t total = 0;

    do {
        size_t decodeLength = 0;
        auto ch = DecodeChar(buffer, end, decodeLength);

        if (ch == InvalidEncoding || ch == InvalidCharacter) {
            break;
        }

        size_t encodeLength = 0;
        UTF8().EncodeChar(ch, tempBuffer, tempBuffer + sizeof(tempBuffer), encodeLength);

        // Assume decoded character is always valid.
        stream.write(tempBuffer, encodeLength);

        buffer = add(buffer, decodeLength);
        total += decodeLength;
    } while (buffer < end);

    used = total;

    return stream.str();
}

size_t Encoding::EncodeString(const std::string& string, void* buffer, const void* end) {
    const auto* S = string.c_str();
    const auto* SE = S + string.length();

    const auto* B = static_cast<const char*>(buffer);
    auto* I = static_cast<char*>(buffer);
    const auto* E = static_cast<const char*>(end);
    char tempBuffer[8];

    do {
        size_t decodeLength = 0;
        auto ch = UTF8().DecodeChar(S, SE, decodeLength);

        if (ch == InvalidEncoding || ch == InvalidCharacter) {
            break;
        }

        size_t encodeLength = 0;
        if (!EncodeChar(ch, tempBuffer, tempBuffer + sizeof(tempBuffer), encodeLength)) {
            break;
        }

        // Range check
        if (I + encodeLength >= E) {
            break;
        }

        S += decodeLength;
        I += encodeLength;
    } while (S < SE && I < E);

    return (I - B);
}

// static
void Encoding::Register(Encoding* encoding) {
    GetEncodingMap().insert({ encoding->GetName(), encoding });
}

// static
Encoding* Encoding::GetEncoding(const std::string& name) {
    auto it = GetEncodingMap().find(name);

    return it != GetEncodingMap().end() ? it->second : nullptr;
}