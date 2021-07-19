#
#  ConvertTestGen.py
#  - Test data generator for ConvertInteger and ConvertFloat.
#
#  Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
#  Released under the MIT License.
#  See the LICENSE file in the project root to get more information.
#
import random
from os.path import basename, join
from shared import header, PROJECT_ROOT
import sys


SELF_NAME = basename(__file__)
MAX_COUNT = 100


def random_int():
    min_val, max_val = random.choice([
        (0, 10),         # ten
        (10, 100),        # one hundred
        (100, 1000),       # one thousand
        (1000, 10000),      # ten thousand
        (10000, 100000),     # one hundred thousand
        (100000, 1000000),    # one million
        (1000000, 10000000),   # ten million
        (10000000, 100000000),  # hundred million
        (100000000, 1000000000)  # one billion
    ])

    return random.randint(min_val, max_val)


def scientific_generator():
    return '{0}{1}{2}'.format(
        random_int(), random.choice(['e', 'E']), random.randint(-100, 100)
    )


def fixed_generator():
    return '{0}.{1}'.format(random_int(), random_int())


def mixed_generator():
    return '{0}.{1}{2}{3}'.format(
        random_int(), random_int(), random.choice(['e', 'E']), random.randint(-309, 308)
    )


def int_generator():
    return str(random_int())


def generator():
    generators = [scientific_generator, fixed_generator, mixed_generator, int_generator]

    while True:
        yield random.choice(generators)()


def main():
    float_output = join(PROJECT_ROOT, 'Test/Utils/ConvertTest.Float.h')

    with header(float_output, SELF_NAME, 'CONVERTTEST_FLOAT_H') as f:
        f.write('const struct {\n')
        f.write('    const char* String;\n')
        f.write('    double Expect;\n')
        f.write('} TestData[] = {\n')

        index = 0
        get_data = generator()
        while index < MAX_COUNT:
            data = next(get_data)
            if sys.float_info.max < float(data) or sys.float_info.min > float(data):
                continue

            f.write('    {{ "{0}",  {1} }},\n'.format(data, float(data).hex()))
            index += 1

        f.write('};\n')

    int_output = join(PROJECT_ROOT, 'Test/Utils/ConvertTest.Int.h')

    with header(int_output, SELF_NAME, 'CONVERTTEST_INT_H') as f:
        pass


if __name__ == '__main__':
    main()
