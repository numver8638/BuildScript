/*
 * ConvertTest.cpp
 * - Test cases for ConvertInteger and ConvertFloat.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <catch2/catch.hpp>

#include <BuildScript/Utils/Convert.h>
#include <BuildScript/Utils/BitCast.h>

using namespace BuildScript;

TEST_CASE("Convert Floating Point", "[Convert]") {
    #include "ConvertTest.Float.h"

    int index = 1;
    for (const auto& data : TestData) {
        bool error;
        double value;

        INFO("index: " << index++);

        std::tie(error, value) = BuildScript::ConvertFloat(data.String);

        REQUIRE(!error);
        REQUIRE(value == Approx(data.Expect).epsilon(0));
    }
}
