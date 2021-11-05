// This file is machine generated by EncodingTestGen.py at Tue Sep 14 12:35:34 2021. DO NOT EDIT.
#ifndef TEST_UTILS_ENCODINGTEST_UTF16BE_H
#define TEST_UTILS_ENCODINGTEST_UTF16BE_H

// Decode Test Data
const char RandomDecodeData[] = {
    '\xDD',
    '\xA2',
    '\x3C',
    '\xF2',
    '\x48',
    '\x86',
    '\xDD',
    '\x3B',
    '\xCE',
    '\x1D',
    '\xE0',
    '\x72',
    '\x71',
    '\x28',
    '\x13',
    '\x3B',
    '\xDD',
    '\x5D',
    '\x3C',
    '\xD5',
    '\x11',
    '\x2F',
    '\xC5',
    '\xEA',
    '\xC6',
    '\xBB',
    '\xDD',
    '\xD2',
    '\x97',
    '\x85',
    '\xAE',
    '\x78',
    '\xDB',
    '\x08',
    '\xCE',
    '\xD0',
    '\x36',
    '\x0A',
    '\xEA',
    '\x87',
    '\x08',
    '\x22',
    '\xD8',
    '\x47',
    '\xDD',
    '\x56',
    '\x07',
    '\xE8',
    '\x28',
    '\xA7',
    '\x07',
    '\xEA',
    '\xD6',
    '\x2B',
    '\xDC',
    '\xA9',
    '\xB1',
    '\x75',
    '\x37',
    '\x46',
    '\x5C',
    '\x68',
    '\xDE',
    '\x4F',
    '\x09',
    '\x79',
    '\xD9',
    '\x45',
    '\xDE',
    '\x4F',
    '\x65',
    '\xFC',
    '\x00',
    '\x4D',
    '\x74',
    '\xCF',
    '\x0A',
    '\x5F',
    '\xDB',
    '\x85',
    '\xDD',
    '\xBB',
    '\x8A',
    '\xAD',
    '\xCD',
    '\x3A',
    '\x55',
    '\x76',
    '\x17',
    '\x58',
    '\xD8',
    '\x18',
    '\xDE',
    '\xFD',
    '\x9B',
    '\x69',
};

const char32_t RandomDecodeExpects[] = {
    Encoding::InvalidEncoding,
    static_cast<char32_t>(0x00003CF2),
    static_cast<char32_t>(0x00004886),
    Encoding::InvalidEncoding,
    static_cast<char32_t>(0x0000CE1D),
    static_cast<char32_t>(0x0000E072),
    static_cast<char32_t>(0x00007128),
    static_cast<char32_t>(0x0000133B),
    Encoding::InvalidEncoding,
    static_cast<char32_t>(0x00003CD5),
    static_cast<char32_t>(0x0000112F),
    static_cast<char32_t>(0x0000C5EA),
    static_cast<char32_t>(0x0000C6BB),
    Encoding::InvalidEncoding,
    static_cast<char32_t>(0x00009785),
    static_cast<char32_t>(0x0000AE78),
    Encoding::InvalidEncoding,
    static_cast<char32_t>(0x0000CED0),
    static_cast<char32_t>(0x0000360A),
    static_cast<char32_t>(0x0000EA87),
    static_cast<char32_t>(0x00000822),
    static_cast<char32_t>(0x00021D56),
    static_cast<char32_t>(0x000007E8),
    static_cast<char32_t>(0x000028A7),
    static_cast<char32_t>(0x000007EA),
    static_cast<char32_t>(0x0000D62B),
    Encoding::InvalidEncoding,
    static_cast<char32_t>(0x0000B175),
    static_cast<char32_t>(0x00003746),
    static_cast<char32_t>(0x00005C68),
    Encoding::InvalidEncoding,
    static_cast<char32_t>(0x00000979),
    static_cast<char32_t>(0x0006164F),
    static_cast<char32_t>(0x000065FC),
    static_cast<char32_t>(0x0000004D),
    static_cast<char32_t>(0x000074CF),
    static_cast<char32_t>(0x00000A5F),
    static_cast<char32_t>(0x000F15BB),
    static_cast<char32_t>(0x00008AAD),
    static_cast<char32_t>(0x0000CD3A),
    static_cast<char32_t>(0x00005576),
    static_cast<char32_t>(0x00001758),
    static_cast<char32_t>(0x000162FD),
    static_cast<char32_t>(0x00009B69),
};

constexpr size_t RandomDecodeExpectChars = 37;
constexpr size_t RandomDecodeExpectErrors = 7;

// Encode Test Data
const char32_t RandomEncodeData[] = {
    static_cast<char32_t>(0x00008A46),
    static_cast<char32_t>(0x0000459F),
    static_cast<char32_t>(0x00002470),
    static_cast<char32_t>(0x00006F0A),
    static_cast<char32_t>(0x0000620A),
    static_cast<char32_t>(0x000D4D2B),
    static_cast<char32_t>(0x0000D90A),
    static_cast<char32_t>(0x0000BE19),
    static_cast<char32_t>(0x0000991B),
    static_cast<char32_t>(0x000041A0),
    static_cast<char32_t>(0x00002FCC),
    static_cast<char32_t>(0x0000856D),
    static_cast<char32_t>(0x00000F4F),
    static_cast<char32_t>(0xF778734D),
    static_cast<char32_t>(0x0000FA7D),
    static_cast<char32_t>(0x0000ED43),
    static_cast<char32_t>(0x0000BE4C),
    static_cast<char32_t>(0x00001569),
    static_cast<char32_t>(0xDD7AE475),
    static_cast<char32_t>(0x00002B0B),
    static_cast<char32_t>(0x00003920),
    static_cast<char32_t>(0x00006469),
    static_cast<char32_t>(0x000023D0),
    static_cast<char32_t>(0x000163C3),
    static_cast<char32_t>(0x00007FD4),
    static_cast<char32_t>(0x0000787C),
    static_cast<char32_t>(0xED435CFE),
    static_cast<char32_t>(0x000087E3),
    static_cast<char32_t>(0x0000BF7A),
    static_cast<char32_t>(0x00004B3C),
    static_cast<char32_t>(0x00000EB2),
    static_cast<char32_t>(0x0000AC06),
    static_cast<char32_t>(0x0000CCAF),
    static_cast<char32_t>(0x0000785E),
    static_cast<char32_t>(0x000E4623),
};

const struct {
    size_t Length;
    char Data[8];
} RandomEncodeExpects[] = {
    { 2, { '\x8A', '\x46' } },
    { 2, { '\x45', '\x9F' } },
    { 2, { '\x24', '\x70' } },
    { 2, { '\x6F', '\x0A' } },
    { 2, { '\x62', '\x0A' } },
    { 4, { '\xDB', '\x13', '\xDD', '\x2B' } },
    { 0, {  } },
    { 2, { '\xBE', '\x19' } },
    { 2, { '\x99', '\x1B' } },
    { 2, { '\x41', '\xA0' } },
    { 2, { '\x2F', '\xCC' } },
    { 2, { '\x85', '\x6D' } },
    { 2, { '\x0F', '\x4F' } },
    { 0, {  } },
    { 2, { '\xFA', '\x7D' } },
    { 2, { '\xED', '\x43' } },
    { 2, { '\xBE', '\x4C' } },
    { 2, { '\x15', '\x69' } },
    { 0, {  } },
    { 2, { '\x2B', '\x0B' } },
    { 2, { '\x39', '\x20' } },
    { 2, { '\x64', '\x69' } },
    { 2, { '\x23', '\xD0' } },
    { 4, { '\xD8', '\x18', '\xDF', '\xC3' } },
    { 2, { '\x7F', '\xD4' } },
    { 2, { '\x78', '\x7C' } },
    { 0, {  } },
    { 2, { '\x87', '\xE3' } },
    { 2, { '\xBF', '\x7A' } },
    { 2, { '\x4B', '\x3C' } },
    { 2, { '\x0E', '\xB2' } },
    { 2, { '\xAC', '\x06' } },
    { 2, { '\xCC', '\xAF' } },
    { 2, { '\x78', '\x5E' } },
    { 4, { '\xDB', '\x51', '\xDE', '\x23' } },
};

constexpr size_t RandomEncodeExpectChars = 31;
constexpr size_t RandomEncodeExpectErrors = 4;

#endif // TEST_UTILS_ENCODINGTEST_UTF16BE_H