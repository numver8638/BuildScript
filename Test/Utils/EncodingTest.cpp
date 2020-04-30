/*
 * EncodingTest.cpp
 * - Test for Encoding.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <catch2/catch.hpp>

#include <iostream>

#include <BuildScript/Utils/Encoding.h>

using namespace BuildScript;

#define RANDOM_CHARACTER_TEST \
        do { \
            const char* begin = Data; \
            const char* end = begin + sizeof(Data); \
            const char* buffer = begin; \
            auto count = 0; \
            auto index = 0; \
            do { \
                auto ch = encoding->DecodeChar(buffer, end, index); \
                REQUIRE(ExpectData[count] == ch); \
                buffer += index; \
                count++; \
            } while (buffer < end); \
            REQUIRE(count == CharCount); \
        } while (false)

#define INVALID_CHARACTER_TEST \
        do { \
            const char* begin = Data; \
            const char* end = begin + sizeof(Data); \
            const char* buffer = begin; \
            auto errors = 0; \
            auto index = 0; \
            do { \
                auto ch = encoding->DecodeChar(buffer, end, index); \
                if (ch == Encoding::InvalidCharacter || ch == Encoding::InvalidEncoding) \
                    errors++; \
                buffer += index; \
            } while (buffer < end); \
            REQUIRE(errors > 0); \
        } while (false)

TEST_CASE("UTF-8 Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf8");

    REQUIRE(encoding != nullptr);

    SECTION("Random character decoding") {
        #include "EncodingTest/UTF8.Random.h"
        RANDOM_CHARACTER_TEST;
    }

    SECTION("Invalid character decoding") {
        #include "EncodingTest/UTF8.Invalid.h"
        INVALID_CHARACTER_TEST;
    }
}

TEST_CASE("UTF-16 Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf16");

    REQUIRE(encoding != nullptr);

    SECTION("Random character decoding") {
        #include "EncodingTest/UTF16.Random.h"
        RANDOM_CHARACTER_TEST;
    }

    SECTION("Invalid character decoding") {
        #include "EncodingTest/UTF16.Invalid.h"
        INVALID_CHARACTER_TEST;
    }
}

TEST_CASE("UTF-16 BE Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf16be");

    REQUIRE(encoding != nullptr);

    SECTION("Random character decoding") {
        #include "EncodingTest/UTF16BE.Random.h"
        RANDOM_CHARACTER_TEST;
    }

    SECTION("Invalid character decoding") {
        #include "EncodingTest/UTF16BE.Invalid.h"
        INVALID_CHARACTER_TEST;
    }
}

TEST_CASE("UTF-32 Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf32");

    REQUIRE(encoding != nullptr);

    SECTION("Random character decoding") {
        #include "EncodingTest/UTF32.Random.h"
        RANDOM_CHARACTER_TEST;
    }

    SECTION("Invalid character decoding") {
        #include "EncodingTest/UTF32.Invalid.h"
        INVALID_CHARACTER_TEST;
    }
}

TEST_CASE("UTF-32 BE Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("utf32be");

    REQUIRE(encoding != nullptr);

    SECTION("Random character decoding") {
        #include "EncodingTest/UTF32BE.Random.h"
        RANDOM_CHARACTER_TEST;
    }

    SECTION("Invalid character decoding") {
        #include "EncodingTest/UTF32BE.Invalid.h"
        INVALID_CHARACTER_TEST;
    }
}

TEST_CASE("EUC-KR Test", "[Encoding]") {
    Encoding* encoding = Encoding::GetEncoding("euckr");

    REQUIRE(encoding != nullptr);

    SECTION("Random character decoding") {
        #include "EncodingTest/EUCKR.Random.h"
        RANDOM_CHARACTER_TEST;
    }

    SECTION("Invalid character decoding") {
        #include "EncodingTest/EUCKR.Invalid.h"
        INVALID_CHARACTER_TEST;
    }
}