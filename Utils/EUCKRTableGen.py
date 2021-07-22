#
#  EUCKRTableGen.py
#  - Conversion table generator for EUC-KR encoding.
#
#  Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
#  Released under the MIT License.
#  See the LICENSE file in the project root to get more information.
#
from os.path import basename, join
import re
from shared import PROJECT_ROOT, header

SELF_NAME = basename(__file__)

UNIFIED_HANGUL = 1
UNICODE = 2
REGEX = re.compile(r'0x([0-9A-F]{4})\t0x([0-9A-F]{4})\t(.)+')

CP949_TO_UNICODE = [None for _ in range(256)]
UNICODE_TO_CP949 = [None for _ in range(0x10000)]


def update_table(cp949: int, unicode: int):
    cp949_lead = (cp949 >> 8) & 0xFF
    cp949_trial = cp949 & 0xFF

    if CP949_TO_UNICODE[cp949_lead] is None:
        CP949_TO_UNICODE[cp949_lead] = [0 for _ in range(0, 256)]

    CP949_TO_UNICODE[cp949_lead][cp949_trial] = unicode
    UNICODE_TO_CP949[unicode] = cp949


def process(source):
    with header(source, SELF_NAME, 'UTILS_ENCODING_EUCKR_CONVERTTABLE_H') as f:
        f.write('// Origin: https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP949.TXT\n\n')

        for e, index in zip(CP949_TO_UNICODE, range(256)):
            if e is not None:
                f.write('static const char16_t EUCKRLeadByte_{0:X}[256] = {{'.format(index))
                for ch, chars in zip(e, range(256)):
                    if chars % 16 == 0:
                        f.write('\n    ')
                    f.write('0x{0:04X}, '.format(ch))
                f.write('\n};\n\n')

        f.write('static const char16_t *EUCKRToUnicode[256] = {')
        for e, index in zip(CP949_TO_UNICODE, range(256)):
            if index % 16 == 0:
                f.write('\n    ')
            if e is None:
                f.write('nullptr,          ')
            else:
                f.write('EUCKRLeadByte_{0:X}, '.format(index))
        f.write('\n};\n\n')

        f.write('static const char16_t UnicodeToEUCKR[0x10000] = {')
        for e, index in zip(UNICODE_TO_CP949, range(0x10000)):
            if index % 16 == 0:
                f.write('\n    ')
            if e is None:
                f.write('0x0000, ')
            else:
                f.write('0x{0:04X}, '.format(e))
        f.write('\n};')


def main():
    db = join(PROJECT_ROOT, 'Utils/CP949.TXT')

    with open(db) as f:
        for line in f:
            matches = REGEX.search(line)

            if matches is not None:
                cp949 = matches.group(UNIFIED_HANGUL)
                uni = matches.group(UNICODE)
                update_table(int(cp949, base=16), int(uni, base=16))

    source = join(PROJECT_ROOT, 'Source/Utils/Encoding/EUCKR.ConversionTable.h')
    process(source)


if __name__ == '__main__':
    main()
