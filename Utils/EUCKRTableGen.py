#
#  EUCKRTableGen.py
#  - Conversion table generator for EUC-KR encoding.
#
#  Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
#  Released under the MIT License.
#  See the LICENSE file in the project root to get more information.
#
import re
import time
import os

CP949_TO_UNICODE = [None for _ in range(0, 256)]
PROJECT_ROOT = os.path.dirname(os.path.dirname(__file__))

def update_table(cp949, uni):
    decoded_unicode = int(uni, base=16)
    decoded_cp949 = int(cp949, base=16)
    cp949_lead = (decoded_cp949 >> 8) & 0xFF
    cp949_trial = decoded_cp949 & 0xFF
    
    if CP949_TO_UNICODE[cp949_lead] == None:
        CP949_TO_UNICODE[cp949_lead] = [0 for _ in range(0, 256)]

    CP949_TO_UNICODE[cp949_lead][cp949_trial] = decoded_unicode

def process():
    source = os.path.join(PROJECT_ROOT, 'Source/Utils/Encoding/EUCKR.ConversionTable.cpp')
    with open(source, 'w') as f:
        f.write('// This file is machine generated file by EUCKRTableGen.py at {0}. DO NOT EDIT.\n// Origin: https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP949.TXT\n\n'.format(time.asctime()))
        
        for (e, index) in zip(CP949_TO_UNICODE, range(0, 256)):
            if e != None:
                f.write('static const char16_t EUCKRLeadByte_{0:X}[256] = {{'.format(index))
                for (ch, chars) in zip(e, range(0, 256)):
                    if chars % 16 == 0:
                        f.write('\n    ')
                    f.write('0x{0:04X}, '.format(ch))
                f.write('\n};\n\n')

        f.write('const char16_t *EUCKRtoUTF8[256] = {')
        for (e, index) in zip(CP949_TO_UNICODE, range(0, 256)):
            if index % 16 == 0:
                f.write('\n    ')
            if e == None:
                f.write('nullptr,          ')
            else:
                f.write('EUCKRLeadByte_{0:X}, '.format(index))
        f.write('\n};')

UNIFIED_HANGUL = 1
UNICODE = 2
REGEX = '0x([0-9A-F]{4})\t0x([0-9A-F]{4})\t(.)+'

regex = re.compile(REGEX)

db = os.path.join(PROJECT_ROOT, 'Utils/CP949.TXT')
with open(db) as f:
    for line in f:
        matches = regex.search(line)

        if matches != None:
            cp949 = matches.group(UNIFIED_HANGUL)
            uni = matches.group(UNICODE)
            update_table(cp949, uni)

process()