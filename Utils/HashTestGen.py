#
#  HashTestGen.py
#  - Test data generator for Hash.
#
#  Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
#  Released under the MIT License.
#  See the LICENSE file in the project root to get more information.
#
import hashlib
from os.path import basename, join
from random import randint, randbytes
from shared import PROJECT_ROOT, header

SELF_NAME = basename(__file__)


def main():
    output = join(PROJECT_ROOT, 'Test/Utils/HashTestSet.h')
    with header(output, SELF_NAME, 'TEST_UTILS_HASHTESTSET_H') as f:
        count = randint(100, 200)

        f.write('#include <array>\n\n')

        f.write(f'constexpr size_t DataSetCount = {count};\n\n')
        f.write('static const struct {\n'
                '    const char* data;\n'
                '    size_t length;\n'
                '    std::array<uint8_t, 20> expect;\n'
                '} TestDataSet[DataSetCount] = {\n')
        for _ in range(count):
            data = randbytes(randint(200, 300))
            stringify = ''.join([f'\\x{b:02X}' for b in data])
            expect = ', '.join([f'0x{b:02X}' for b in hashlib.sha1(data).digest()])

            f.write(f'    {{ "{stringify}", {len(data)}, {{ {expect} }} }},\n')

        f.write('};')


if __name__ == '__main__':
    main()
