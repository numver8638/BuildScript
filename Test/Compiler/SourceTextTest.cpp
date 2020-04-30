/*
 * SourceTextTest.cpp
 * - Test for SourceText.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <catch2/catch.hpp>
#include <BuildScript/Compiler/SourceText.h>

using namespace BuildScript;

constexpr const char* FILENAME = TEST_SRC u8"/Scripts/TestScript.script";
constexpr const char* EMPTY_FILENAME = TEST_SRC u8"/Scripts/EmptyScript.script";
constexpr const char* NONEXIST_FILENAME = u8"__NeverExists__";

#define SKIP_CHARS(count) do { for (auto i = 0; i < (count); i++) source.Consume(); } while (false)

TEST_CASE("OpenFileTest", "[SourceText]") {
    SourceText source(FILENAME);

    REQUIRE(source.PeekChar() == u'0');
}

TEST_CASE("OpenFailTest", "[SourceText]") {
    REQUIRE_THROWS_AS([]() { SourceText source(NONEXIST_FILENAME); }(), std::ios_base::failure);
}

TEST_CASE("EmptyFileTest", "[SourceText]") {
    REQUIRE_NOTHROW([]() {
        SourceText source(EMPTY_FILENAME);

        REQUIRE(source.PeekChar() == SourceText::EndOfFile);
    });
}

TEST_CASE("ConsumeWhileTest", "[SourceText]") {
    SourceText source(FILENAME);

    // 0...2
    SKIP_CHARS(3);

    auto fn = [](int ch) -> bool {
        return ch != u'8';
    };

    auto begin = source.GetPosition(); // Expect to cursor of 3
    auto end = source.ConsumeWhile(fn); // Expect to cursor of 8

    REQUIRE(source.GetString(begin, end) == u8"34567");
}

TEST_CASE("GetStringTest", "[SourceText]") {
    SourceText source(FILENAME);

    // 0...2
    SKIP_CHARS(3);

    auto begin = source.GetPosition(); // Expect to cursor of 3
    REQUIRE(source.PeekChar() == u'3');

    // 3...7
    SKIP_CHARS(5);

    auto end = source.GetPosition(); // Expect to cursor of 8
    REQUIRE(source.PeekChar() == u'8');

    REQUIRE(source.GetString(begin, end) == u8"34567");
}

TEST_CASE("EOFTest", "[SourceText]") {
    SourceText source(FILENAME);

    // Contains 10 chars
    SKIP_CHARS(10);
    
    REQUIRE(source.PeekChar() == SourceText::EndOfFile);
}