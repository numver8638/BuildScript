/*
 * HashTest.cpp
 * - Test cases for Hash.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <catch2/catch.hpp>

#include <BuildScript/Utils/Hash.h>

using namespace BuildScript;

TEST_CASE("SHA1 Test", "[Hash]") {
    SECTION("Known Value Test") {
        {
            SHA1 hash;
            SHA1::Digest expect = { 0x2F, 0xD4, 0xE1, 0xC6, 0x7A, 0x2D, 0x28, 0xFC, 0xED, 0x84, 0x9E, 0xE1, 0xBB, 0x76, 0xE7, 0x39, 0x1B, 0x93, 0xEB, 0x12 };

            hash.Update("The quick brown fox jumps over the lazy dog", 43);
            auto digest = hash.GetDigest();

            REQUIRE(expect == digest);
        }
        {
            SHA1 hash;
            SHA1::Digest expect = { 0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D, 0x32, 0x55, 0xBF, 0xEF, 0x95, 0x60, 0x18, 0x90, 0xAF, 0xD8, 0x07, 0x09 };

            hash.Update("", 0);
            auto digest = hash.GetDigest();

            REQUIRE(digest == expect);
        }
    }

    SECTION("Random Value Test") {
        #include "HashTestSet.h"
        for (const auto& [data, length, expect] : TestDataSet) {
            SHA1 hash;

            hash.Update(data, length);
            auto digest = hash.GetDigest();

            REQUIRE(expect == digest);
        }
    }
}

TEST_CASE("CRC32 Test", "[Hash]") {

}