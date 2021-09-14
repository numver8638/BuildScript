/*
 * EncodingTest.cpp
 * - Test cases for Encoding class.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <catch2/catch.hpp>

#include <BuildScript/Utils/Encoding.h>

using namespace BuildScript;

#define RANDOM_CHARACTER_DECODE_TEST \
    do { \
        const char* buffer = RandomDecodeData; \
        const char* end = buffer + sizeof(RandomDecodeData); \
        size_t used = 0; \
        auto index = 0; \
        auto count = 0; \
        auto error = 0; \
        do { \
            INFO("Current Index: " << (index)); \
            auto ch = encoding->DecodeChar(buffer, end, used); \
            REQUIRE(RandomDecodeExpects[index] == ch); \
            buffer += used; \
            (ch == Encoding::InvalidEncoding || ch == Encoding::InvalidCharacter) ? ++error : ++count; \
            ++index; \
        } while (buffer < end); \
        REQUIRE(count == RandomDecodeExpectChars); \
        REQUIRE(error == RandomDecodeExpectErrors); \
    } while (false)

#define RANDOM_CHARACTER_ENCODE_TEST \
    do { \
        auto count = 0; \
        auto error = 0; \
        auto index = 0; \
        char buffer[8]; \
        for (auto ch : RandomEncodeData) { \
            INFO("Current Index: " << index); \
            size_t length = 0; \
            auto succeed = encoding->EncodeChar(ch, buffer, buffer + sizeof(buffer), length); \
            REQUIRE(RandomEncodeExpects[index].Length == length); \
            REQUIRE(std::memcmp(buffer, RandomEncodeExpects[index].Data, length) == 0); \
            succeed ? ++count : ++error; \
            ++index; \
        } \
        REQUIRE(count == RandomEncodeExpectChars); \
        REQUIRE(error == RandomEncodeExpectErrors); \
    } while (false)

TEST_CASE("UTF8 Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf-8");

    REQUIRE(encoding != nullptr);

    #include "EncodingTest/UTF8.h"

    SECTION("Random character decoding") {
        RANDOM_CHARACTER_DECODE_TEST;
    }

    SECTION("Random character encoding") {
        RANDOM_CHARACTER_ENCODE_TEST;
    }
}

TEST_CASE("UTF16LE Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf-16le");

    REQUIRE(encoding != nullptr);

    #include "EncodingTest/UTF16LE.h"

    SECTION("Random character decoding") {
        RANDOM_CHARACTER_DECODE_TEST;
    }

    SECTION("Random character encoding") {
        RANDOM_CHARACTER_ENCODE_TEST;
    }
}

TEST_CASE("UTF16BE Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf-16be");

    REQUIRE(encoding != nullptr);

    #include "EncodingTest/UTF16BE.h"

    SECTION("Random character decoding") {
        RANDOM_CHARACTER_DECODE_TEST;
    }

    SECTION("Random character encoding") {
        RANDOM_CHARACTER_ENCODE_TEST;
    }
}

TEST_CASE("UTF32LE Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf-32le");

    REQUIRE(encoding != nullptr);

    #include "EncodingTest/UTF32LE.h"

    SECTION("Random character decoding") {
        RANDOM_CHARACTER_DECODE_TEST;
    }

    SECTION("Random character encoding") {
        RANDOM_CHARACTER_ENCODE_TEST;
    }
}

TEST_CASE("UTF32BE Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf-32be");

    REQUIRE(encoding != nullptr);

    #include "EncodingTest/UTF32BE.h"

    SECTION("Random character decoding") {
        RANDOM_CHARACTER_DECODE_TEST;
    }

    SECTION("Random character encoding") {
        RANDOM_CHARACTER_ENCODE_TEST;
    }
}

TEST_CASE("EUCKR Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("euc-kr");

    REQUIRE(encoding != nullptr);

    #include "EncodingTest/EUCKR.h"

    SECTION("Random character decoding") {
        RANDOM_CHARACTER_DECODE_TEST;
    }

    SECTION("Random character encoding") {
        RANDOM_CHARACTER_ENCODE_TEST;
    }
}