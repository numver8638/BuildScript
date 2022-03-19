#
#  KeywordTableGen.py
#  - Perfect hash function generator.
#    Reference: https://www.dre.vanderbilt.edu/~schmidt/PDF/gperf.pdf
#
#  Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
#  Released under the MIT License.
#  See the LICENSE file in the project root to get more information.
#
from shared import PROJECT_ROOT
from collections import defaultdict
from os.path import join
import re
import time

MAX_TRIES = 10000
PATTERN = re.compile(r'(\w+?)\s+(.+?)$')


class HashGiveUp(Exception):
    """
    Raised when all keyword signature indexes are tried but
    failed to find associated values.
    """
    pass


class HashCollide(Exception):
    """
    Raised when hash collides.
    """

    def __init__(self, key1, key2):
        self.key1 = key1
        self.key2 = key2


def fill_list(list, index, start, length):
    """
    Fill list from `index` to `index + length` count up by 1 from `start`.
    """
    for i in range(length):
        list[index + i] = start + i


def generate_keysig_index(min: int, max: int) -> list:
    """
    Generate keyword signature index used in hash algorithm.
    """
    assert 0 < min <= max

    keysig_index = [0]
    max_length = (max - min) + 1

    while True:
        yield keysig_index

        max_value = max if len(keysig_index) > 1 else min
        length = len(keysig_index)

        if keysig_index[-1] < (max_value - 1):
            keysig_index[-1] += 1
        elif length == 1:
            keysig_index.append(0)

            fill_list(keysig_index, 0, 0, length + 1)
        else:
            index = len(keysig_index) - 1

            while index > 0:
                index -= 1

                if keysig_index[index] + 1 != keysig_index[index + 1]:
                    break

            if index > 0:
                fill_list(keysig_index, index, keysig_index[index] + 1, (length - index))
            elif keysig_index[0] < (min - 1):
                fill_list(keysig_index, 0, keysig_index[0] + 1, length)
            elif length < max_length:
                keysig_index.append(0)
                fill_list(keysig_index, 0, 0, length + 1)
            else:
                return


def get_keysig(keyword: str, keysig_index: list) -> str:
    """
    Get keyword signature from keyword and keyword signature index.
    """
    keysig = []
    length = len(keyword)

    for i in keysig_index:
        if i < length:
            keysig.append(keyword[i])

    return ''.join(keysig)


def calculate_hash(keyword: str, keysig_index: list, asso_values: list) -> int:
    """
    Calculate hash of keyword from keyword signature index and associated values
    """
    h = length = len(keyword)

    for i in keysig_index:
        if i < length:
            h += asso_values[ord(keyword[i])]

    return h


def read_inputs(file: str) -> list:
    """
    Read input and convert into data.
    """
    data = []

    with open(file) as f:
        line_no = 0

        for line in f.readlines():
            line_no += 1
            line = line.lstrip()

            if line.startswith('#'):
                # Ignore comment
                continue

            match = PATTERN.search(line)

            if match is None:
                print('unrecognizable line detected at line {}.'.format(line_no))
            else:
                data.append((match.group(1), match.group(2)))

    return data


def gen_hashtable(data: list, asso_values: list, keysig_index: list) -> dict:
    hashes = {}

    # calculate hash
    for key, value in data:
        h = calculate_hash(key, keysig_index, asso_values)

        if h in hashes:
            raise HashCollide(key, hashes[h][0])
        else:
            hashes[h] = (key, value)

    return hashes


def compute_hash(data: list) -> tuple:
    # 입력 키워드 별로 keysig를 구함
    # asso_value가 충돌할 경우 keysig에서 겹치지 않는 부분 중 하나를 선택해서 값을 수정.
    # keysig 사용 빈도 순으로 정렬하여 가장 적게 사용되는 keysig의 asso_value를 업데이트.
    min_len = len(min(data, key=lambda t: len(t[0]))[0])
    max_len = len(max(data, key=lambda t: len(t[0]))[0])

    for keysig in generate_keysig_index(min_len, max_len):
        print(keysig)

        asso_values = [0 for _ in range(128)]
        hashes: dict
        occurrence = defaultdict(int)

        # occurrence
        for key, _ in data:
            key_length = len(key)
            for i in keysig:
                if i < key_length:
                    occurrence[key[i]] += 1

        tries = 0

        while tries < MAX_TRIES:
            tries += 1

            try:
                hashes = gen_hashtable(data, asso_values, keysig)
            except HashCollide as e:
                A = set(get_keysig(e.key1, keysig))
                B = set(get_keysig(e.key2, keysig))

                V = sorted((A | B) - (A & B), key=lambda value: occurrence[value] * 1000 + ord(value))
                target_asso: str

                if len(V) == 0:
                    # Always occurs hash collide. break loop.
                    break
                else:
                    target_asso = V[0]

                asso_values[ord(target_asso)] += 1
            else:
                # Fill asso_values in -1 except occurred characters.
                for i in range(128):
                    if chr(i) not in occurrence.keys():
                        asso_values[i] = -1

                return asso_values, hashes, keysig

    raise HashGiveUp()


TEMPLATE = """/*
 * Lexer.KeywordTable.cpp
 * - Perfect hash table for finding keywords.
 *   This file is machine generated by KeywordTableGen.py at {date}. DO NOT EDIT.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Lexer.h>

using namespace BuildScript;

#define MIN_WORD_LENGTH {min_word_length}
#define MAX_WORD_LENGTH {max_word_length}
#define MAX_HASH_VALUE {max_hash_value}

static size_t GetIndex(const std::string& str) {{
    static const unsigned char asso_values[] = {{
        {asso_values}
    }};

    auto hval = str.length();

    switch (hval) {{
        default:
        {switches}
            break;
    }}

    return hval;
}}

// static
TokenType Lexer::GetKeyword(const std::string& str) {{
    static const struct {{
        const char* Name;
        TokenType   Type;
    }} wordlist[] = {{
        {wordlist}
    }};

    if (str.length() <= MAX_WORD_LENGTH && str.length() >= MIN_WORD_LENGTH) {{
        auto key = GetIndex(str);

        if (key <= MAX_HASH_VALUE) {{
            auto& word = wordlist[key];

            return str == word.Name ? word.Type : TokenType::Identifier;
        }}
    }}

    return TokenType::Identifier;
}}"""


def first_last(length, step):
    start = 0

    while (start + step) < length:
        yield start, start + step
        start += step

    yield start, length


def output(asso_values: list, hashes: dict, keysig_index: list) -> None:
    print(asso_values)
    print(hashes)
    print(keysig_index)

    max_hash_value = max(hashes.keys())
    max_hash_value_length = len(str(max_hash_value))
    min_word_length = len(min(hashes.values(), key=lambda value: len(value[0]))[0])
    max_word_length = len(max(hashes.values(), key=lambda value: len(value[0]))[0])

    asso_values_list = ',\n        '.join([
        ', '.join([
            "{0:>{width}}".format(
                (max_hash_value + 1 if i < 0 else i), width=max_hash_value_length
            ) for i in asso_values[first:last]
        ]) for first, last in first_last(len(asso_values), 10)
    ])

    switches = []

    keysig_max = max(keysig_index)
    for index in reversed(range(keysig_max + 1)):
        if index < keysig_max:
            switches.append('case {0}:'.format(index + 1))

        if index in keysig_index:
            switches.append('    hval += asso_values[static_cast<char>(str[{0}])];'.format(index))
            switches.append('    /*[[fallthrough]]*/')

    switches_text = '\n        '.join(switches)

    wordlist = '\n        '.join([
        '{{ "{0}", {1} }},'.format(hashes[index][0], hashes[index][1])
        if index in hashes
        else '{ "", TokenType::Identifier },'
        for index in range(max_hash_value + 1)
    ])

    with open(join(PROJECT_ROOT, 'Source/Compiler/Parse/Lexer.KeywordTable.cpp'), 'w') as f:
        f.write(
            TEMPLATE.format(date=time.asctime(), min_word_length=min_word_length, max_word_length=max_word_length,
                            max_hash_value=max_hash_value, asso_values=asso_values_list, switches=switches_text,
                            wordlist=wordlist)
        )


def main():
    try:
        print('Read keywords...')
        data = read_inputs(join(PROJECT_ROOT, 'Utils/Keywords'))
        print('Generating hashes...')
        (asso_values, hashes, keysig_index) = compute_hash(data)
        print('Write files...')
        output(asso_values, hashes, keysig_index)
    except HashGiveUp:
        print('Hash generation failed.')
    else:
        print('Generation complete.')


if __name__ == '__main__':
    main()
