/*
 * Convert.cpp
 * - Utility functions for converting integers and floating point numbers.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Utils/Convert.h>

#include <BuildScript/Utils/BitCast.h>

constexpr char ToUpper(char ch) {
    return char(ch & ~0x20);
}

constexpr bool IsDecimal(char ch) {
    return ('0' <= ch) && (ch <= '9');
}

namespace BuildScript {
    // This assumes that input string is always well-formed and no leading and trailing whitespaces.
    std::tuple<bool, int64_t> ConvertInteger(const std::string& str) {
        auto base = 10;
        int64_t value = 0;
        const auto* S = str.c_str();

        if (str.length() > 2 && str[0] == '0') {
            switch (str[1]) {
                case 'b': case 'B':
                    base = 2;
                    break;

                case 'o': case 'O':
                    base = 8;
                    break;

                case 'x': case 'X':
                    base = 16;
                    break;

                default:
                    // Do nothing.
                    break;
            }

            if (base != 10) {
                S += 2;
            }
        }

        while (*S != '\0') {
            int val;

            if (IsDecimal(*S)) {
                val = *S - '0';
            }
            else {
                val = ToUpper(*S) - 'A';
            }

            // Check overflow
            if (value < 0) {
                return { /*overflow=*/true, 0 };
            }

            value = (value * base) + val;
            S++;
        }

        return { /*overflow=*/false, value };
    }

    constexpr double Infinity = std::numeric_limits<double>::infinity();
    constexpr uint64_t MaxValue = 1000000000000000000UL;
    constexpr int MinDecimalExp = std::numeric_limits<double>::min_exponent10;
    constexpr int MaxDecimalExp = std::numeric_limits<double>::max_exponent10;
    constexpr int MinBinaryExp = std::numeric_limits<double>::min_exponent;
    constexpr int MaxBinaryExp = std::numeric_limits<double>::max_exponent;
    constexpr uint64_t High4Bits = 0xF000000000000000ul;
    constexpr uint64_t HighBit = 0x8000000000000000ul;

    constexpr uint64_t FractionMask = 0x7FFFFFFFFFFFF800ul; // Ignores MSB.
    constexpr int FractionShift = 52;
    constexpr int Bias = 1023; // 2^10 - 1
    constexpr uint64_t ExponentMask = 0x7FF;


    // This assumes that input string is always well-formed and no leading and trailing whitespaces.
    // Origin: http://krashan.ppa.pl/articles/stringtofloat/
    std::tuple<bool, double> ConvertFloat(const std::string& str) {
        auto* S = str.c_str();
        uint64_t value = 0;
        int exp = 0;

        // 1. Skip leading zeros that is meaningless.
        while (*S == '0') {
            ++S;
        }

        // 2. Consume integer part of mantissa
        while (IsDecimal(*S)) {
            value = value * 10 + (*S++ - '0');
        }

        // 3. Consume fraction part of mantissa.
        if (*S == '.') {
            ++S; // consume .

            while (IsDecimal(*S)) {
                --exp;
                value = value * 10 + (*S++ - '0');
            }
        }

        // 4. Consume exponent part of mantissa.
        if (*S == 'e' || *S == 'E') {
            bool negate = false;
            ++S;

            if (*S == '+' || *S == '-') {
                negate = (*S++ == '-');
            }

            int E = 0;
            while (IsDecimal(*S)) {
                E = E * 10 + (*S++ - '0');
            }

            exp += negate ? -E : E;
        }

        // Check validity.
        if (value == 0) {
            // Zero
            return { /*error=*/false, 0.0 };
        }
        else if (exp > MaxDecimalExp || value > MaxValue) {
            // Overflow
            return { /*error=*/true, Infinity };
        }
        else if (exp < MinDecimalExp) {
            // Underflow
            return { /*error=*/true, 0.0 };
        }

        // Convert into floating point number
        int64_t binexp = 0;

        while (exp > 0) {
            // Overflow check
            while (value & High4Bits) {
                bool round = (value % 2 == 1);
                value >>= 1;

                // ties to even
                if (round && (value % 2 == 1)) { ++value; }

                ++binexp;
            }

            value *= 10;
            --exp;
        }

        while (exp < 0) {
            while (!(value & HighBit)) {
                value <<= 1;
                --binexp;
            }

            auto rem = value % 10;
            value /= 10;

            // Round to nearest, ties to even.
            if (rem > 5 || (rem == 5 && (value % 2 == 1))) {
                ++value;
            }

            ++exp;
        }

        // Shift left until Highest bit is set.
        int shift = 0;
        while (!(value & HighBit)) {
            value <<= 1;
            ++shift;
        }

        // Calibrate binary exponent.
        binexp += (63 - shift);

        if ((value & 0x7FF) > 0x400) {
            // Round to nearest
            value += 0x800;
        }
        else if ((value & 0x7FF) == 0x400) {
            // Ties to even
            // Converted value is always even when binexp is less than zero or grater than 52.
            if (0 <= binexp && binexp <= 52) {
                if ((value >> (63 - binexp)) % 2 == 1) {
                    // converted value is odd.
                    value += 0x800;
                }
            }
        }

        // Range check of binary exponent.
        if (binexp < MinBinaryExp) {
            // Underflow
            return { /*error=*/true, 0.0 };
        }
        else if (binexp > MaxBinaryExp) {
            // Overflow
            return { /*error=*/true, Infinity };
        }
        else {
            // Normal value
            uint64_t result = (value & FractionMask) >> 11 | ((binexp + Bias) & ExponentMask) << FractionShift;
            return { /*error=*/false, bit_cast<double>(result) };
        }
    }
}