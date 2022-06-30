# Opcodes
This document describes instructions and instruction format that BuildScript's VM understands.

## Opcode Format

- op: 1 byte (unsigned)
- reg: 1 byte (unsigned)
- u8: 1 byte (unsigned)
- uint: 2 bytes (unsigned)
- sint: 2 bytes (signed)
- abs_addr: 4 bytes (unsigned)
- rel_addr: 2 bytes (unsigned)

| Type    | Size                | Format                                         |
|---------|---------------------|------------------------------------------------|
| Type 1  | 1 byte              | `op`                                           |
| Type 2  | 2 bytes             | `op reg`                                       |
| Type 3  | 2 bytes             | `op reg u8`                                    |
| Type 4  | 3 bytes             | `op reg reg`                                   |
| Type 5  | 4 bytes             | `op reg uint`                                  |
| Type 6  | 5 bytes             | `op uint uint`                                 |
| Type 7  | 4 bytes             | `op reg sint`                                  |
| Type 8  | 5 bytes             | `op reg reg uint`                              |
| Type 9  | 4 bytes             | `op reg reg reg`                               |
| Type 10 | 5 bytes             | `op test_op reg reg reg`                       |
| Type 11 | 5 bytes             | `op abs_addr`                                  |
| Type 12 | 4 bytes             | `op reg rel_addr`                              |
| Type 13 | 8 + 6 * count bytes | `op reg uint abs_addr (uint abs_addr) * count` |

## Instructions
| Mnemonic         | Opcode | Type    | Description                                                                                             |
|------------------|--------|---------|---------------------------------------------------------------------------------------------------------|
| NOP              | 0x00   | Type 1  | _NOP_                                                                                                   |
| BREAK            | 0x01   | Type 1  | _Call the debugger_                                                                                     |
| MOV              | 0x02   | Type 4  | _reg_A_ = _reg_B_                                                                                       |
| LOAD_GLOBAL      | 0x03   | Type 5  | _reg_A_ = _GLOBAL[CONST[index]]_                                                                        |
| LOAD_CONST       | 0x04   | Type 5  | _reg_A_ = _CONST[index]_                                                                                |
| LOAD_INT         | 0x05   | Type 7  | _reg_A_ = _sint_                                                                                        |
| LOAD_TRUE        | 0x06   | Type 2  | _reg_A_ = _true_                                                                                        |
| LOAD_FALSE       | 0x07   | Type 2  | _reg_A_ = _false_                                                                                       |
| LOAD_NONE        | 0x08   | Type 2  | _reg_A_ = _none_                                                                                        |
| LOAD_ARG         | 0x09   | Type 5  | _reg_A_ = _PARAMS[index]_                                                                               |
| LOAD_RETURN      | 0x0A   | Type 2  | _reg_A_ = _RETURN_                                                                                      |
| LOAD_EXC         | 0x0B   | Type 2  | _reg_A_ = _EXC_                                                                                         |
| STORE_GLOBAL     | 0x0C   | Type 5  | _GLOBAL[index]_ = _reg_A_                                                                               |
| PUSH_ARG         | 0x0D   | Type 2  | _PARAMS.push(reg_A)_                                                                                    |
| ADD              | 0x0E   | Type 9  | _reg_A_ = _reg_B_ + _reg_C_                                                                             |
| SUB              | 0x0F   | Type 9  | _reg_A_ = _reg_B_ - _reg_C_                                                                             |
| MUL              | 0x10   | Type 9  | _reg_A_ = _reg_B_ * _reg_C_                                                                             |
| DIV              | 0x11   | Type 9  | _reg_A_ = _reg_B_ / _reg_C_                                                                             |
| MOD              | 0x12   | Type 9  | _reg_A_ = _reg_B_ % _reg_C_                                                                             |
| SHR              | 0x13   | Type 9  | _reg_A_ = _reg_B_ >> _reg_C_                                                                            |
| SHL              | 0x14   | Type 9  | _reg_A_ = _reg_B_ << _reg_C_                                                                            |
| AND              | 0x15   | Type 9  | _reg_A_ = _reg_B_ & _reg_C_                                                                             |
| OR               | 0x16   | Type 9  | _reg_A_ = _reg_B_ \                                                                                     | _reg_C_                                 |
| XOR              | 0x17   | Type 9  | _reg_A_ = _reg_B_ ^ _reg_C_                                                                             |
| NOT              | 0x18   | Type 4  | _reg_A_ = !_reg_B_                                                                                      |
| NEG              | 0x19   | Type 4  | _reg_A_ = -_reg_B_                                                                                      |
| INPLACE_ADD      | 0x1A   | Type 4  | _reg_A_ += _reg_B_                                                                                      |
| INPLACE_SUB      | 0x1B   | Type 4  | _reg_A_ -= _reg_B_                                                                                      |
| INPLACE_MUL      | 0x1C   | Type 4  | _reg_A_ *= _reg_B_                                                                                      |
| INPLACE_DIV      | 0x1D   | Type 4  | _reg_A_ /= _reg_B_                                                                                      |
| INPLACE_MOD      | 0x1E   | Type 4  | _reg_A_ %= _reg_B_                                                                                      |
| INPLACE_SHR      | 0x1F   | Type 4  | _reg_A_ >>= _reg_B_                                                                                     |
| INPLACE_SHL      | 0x20   | Type 4  | _reg_A_ <<= _reg_B_                                                                                     |
| INPLACE_AND      | 0x21   | Type 4  | _reg_A_ &= _reg_B_                                                                                      |
| INPLACE_OR       | 0x22   | Type 4  | _reg_A_ \                                                                                               |= _reg_B_  _reg_C_                                |
| INPLACE_XOR      | 0x23   | Type 4  | _reg_A_ ^= _reg_B_                                                                                      |
| TEST             | 0x24   | Type 10 | _reg_A_ = _reg_B_ _test_op_ _reg_C_                                                                     |
| DEFINED_GLOBAL   | 0x25   | Type 5  | _reg_A_ = _GLOBAL.Contains(CONST[index])_                                                               |
| DEFINED_PROPERTY | 0x26   | Type 8  | _reg_A_ = _reg_B.HasProperty(CONST[index])_                                                             |
| GET_MEMBER       | 0x27   | Type 8  | _reg_A_ = _reg_B.'CONST[index]'_                                                                        |
| GET_SUBSCRIPT    | 0x28   | Type 9  | _reg_A_ = _reg_B[reg_C]_                                                                                |
| GET_FIELD        | 0x29   | Type 5  | _reg_A_ = _BOUND[index]_                                                                                |
| GET_SUPER        | 0x2A   | Type 5  | _reg_A_ = _BOUND[index]_                                                                                |
| GET_BOUND        | 0x2B   | Type 5  | _reg_A_ = _BOUND[index]_                                                                                |
| SET_MEMBER       | 0x2C   | Type 8  | _reg_B_.'CONST[index]'_ = _reg_A_                                                                       |
| SET_SUBSCRIPT    | 0x2D   | Type 9  | _reg_B_[reg_C] = _reg_A_                                                                                |
| SET_FIELD        | 0x2E   | Type 8  | _reg_B_.'CONST[index]'_ = _reg_A_                                                                       |
| BR               | 0x2F   | Type 11 | _Jump to abs_addr_                                                                                      |
| BR_TRUE          | 0x30   | Type 12 | _Jump to ip + rel_addr if reg is true_                                                                  |
| BR_FALSE         | 0x31   | Type 12 | _Jump to ip + rel_addr if reg is false_                                                                 |
| JUMP_TABLE       | 0x32   | Type 13 | _IP_ = _JUMP_TABLE[reg]_                                                                                |
| CALL             | 0x33   | Type 2  | _reg_A(PARAMS)_                                                                                         |
| INVOKE           | 0x34   | Type 5  | _reg_A.'CONST[index]'(PARAMS)_                                                                          |
| RETURN           | 0x35   | Type 2  | _RETURN_ = _reg_A_                                                                                      |
| RETURN_NONE      | 0x36   | Type 1  | _RETURN_ = _none_                                                                                       |
| RAISE            | 0x37   | Type 2  | _RAISE_ _reg_A_                                                                                         |
| ASSERT           | 0x38   | Type 2  | _RAISE_ _AssertionFailure(reg_A) if previous test is false_                                             |
| IMPORT           | 0x39   | Type 2  | _IMPORT reg_A_                                                                                          |
| EXPORT           | 0x3A   | Type 5  | _EXPORT[index]_ = _reg_A_                                                                               |
| MAKE_LIST        | 0x3B   | Type 2  | _reg_A_ = _List(PARAMS)_                                                                                |
| MAKE_MAP         | 0x3C   | Type 2  | _reg_A_ = _Map(PARAMS)_                                                                                 |
| MAKE_CLOSURE     | 0x3D   | Type 2  | _reg_A_ = _Closure(PARAMS)_                                                                             |
| DECLARE_GLOBAL   | 0x3E   | Type 8  | _define global variable CONST[index] with value reg_B. reg_A is used as flags in this instruction._     |
| DECLARE_FUNCTION | 0x3F   | Type 6  | _define function CONST[index1] in global scope with value CONST[index2]._                               |
| DECLARE_CLASS    | 0x40   | Type 6  | _define class CONST[index1] in global scope with value CONST[index2]._                                  |
| DECLARE_FIELD    | 0x41   | Type 8  | _initialize CONST[index] in static field with value reg_B. reg_A is used as flags in this instruction._ |

## Test Op
| Mnemonic       | Opcode |
|----------------|--------|
| GRATER         | 0      |
| GRATER_EQUAL   | 1      |
| LESS           | 2      |
| LESS_EQUAL     | 3      |
| EQUAL          | 4      |
| NOT_EQUAL      | 5      |
| EQUAL_TYPE     | 6      |
| NOT_EQUAL_TYPE | 7      |
| CONTAIN        | 8      |
| NOT_CONTAIN    | 9      |