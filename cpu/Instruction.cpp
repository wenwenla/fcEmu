#include "Instruction.h"


std::string Instruction::getName() const {
    return _name;
}

InsType Instruction::getType() const {
    return _type;
}

int Instruction::getCycle() const {
    //TODO get cycle via cycle_type
    return _cycle;
}

int Instruction::length() const {
    if(_name[0] == 'B' && _name[1] == 'R' && _name[2] == 'K') {
        return 2;
    }
    int result;
    switch (_type) {
        case InsType::IMM:
        case InsType::ZP:
        case InsType::ZPX:
        case InsType::ZPY:
        case InsType::IZX:
        case InsType::IZY:
        case InsType::REL:
            result = 2;
            break;
        case InsType::ABS:
        case InsType::ABX:
        case InsType::ABY:
        case InsType::IND:
            result = 3;
            break;
        case InsType::IMP:
            result = 1;
            break;
    }
    return result;
}

Instruction::Instruction(const char *name, int cycle, InsType type) {
    _name[0] = name[0];
    _name[1] = name[1];
    _name[2] = name[2];
    _name[3] = '\0';
    _cycle = cycle;
    _type = type;
}

const Instruction Instruction::INS[256] = {
        Instruction("BRK", 7, InsType::IMP), //0x0
        Instruction("ORA", 6, InsType::IZX), //0x1
        Instruction("KIL", 0, InsType::IMP), //0x2
        Instruction("SLO", 8, InsType::IZX), //0x3
        Instruction("NOP", 3, InsType::ZP), //0x4
        Instruction("ORA", 3, InsType::ZP), //0x5
        Instruction("ASL", 5, InsType::ZP), //0x6
        Instruction("SLO", 5, InsType::ZP), //0x7
        Instruction("PHP", 3, InsType::IMP), //0x8
        Instruction("ORA", 2, InsType::IMM), //0x9
        Instruction("ASL", 2, InsType::IMP), //0xa
        Instruction("ANC", 2, InsType::IMM), //0xb
        Instruction("NOP", 4, InsType::ABS), //0xc
        Instruction("ORA", 4, InsType::ABS), //0xd
        Instruction("ASL", 6, InsType::ABS), //0xe
        Instruction("SLO", 6, InsType::ABS), //0xf
        Instruction("BPL", 2, InsType::REL), //0x10
        Instruction("ORA", 5, InsType::IZY), //0x11
        Instruction("KIL", 0, InsType::IMP), //0x12
        Instruction("SLO", 8, InsType::IZY), //0x13
        Instruction("NOP", 4, InsType::ZPX), //0x14
        Instruction("ORA", 4, InsType::ZPX), //0x15
        Instruction("ASL", 6, InsType::ZPX), //0x16
        Instruction("SLO", 6, InsType::ZPX), //0x17
        Instruction("CLC", 2, InsType::IMP), //0x18
        Instruction("ORA", 4, InsType::ABY), //0x19
        Instruction("NOP", 2, InsType::IMP), //0x1a
        Instruction("SLO", 7, InsType::ABY), //0x1b
        Instruction("NOP", 4, InsType::ABX), //0x1c
        Instruction("ORA", 4, InsType::ABX), //0x1d
        Instruction("ASL", 7, InsType::ABX), //0x1e
        Instruction("SLO", 7, InsType::ABX), //0x1f
        Instruction("JSR", 6, InsType::ABS), //0x20
        Instruction("AND", 6, InsType::IZX), //0x21
        Instruction("KIL", 0, InsType::IMP), //0x22
        Instruction("RLA", 8, InsType::IZX), //0x23
        Instruction("BIT", 3, InsType::ZP), //0x24
        Instruction("AND", 3, InsType::ZP), //0x25
        Instruction("ROL", 5, InsType::ZP), //0x26
        Instruction("RLA", 5, InsType::ZP), //0x27
        Instruction("PLP", 4, InsType::IMP), //0x28
        Instruction("AND", 2, InsType::IMM), //0x29
        Instruction("ROL", 2, InsType::IMP), //0x2a
        Instruction("ANC", 2, InsType::IMM), //0x2b
        Instruction("BIT", 4, InsType::ABS), //0x2c
        Instruction("AND", 4, InsType::ABS), //0x2d
        Instruction("ROL", 6, InsType::ABS), //0x2e
        Instruction("RLA", 6, InsType::ABS), //0x2f
        Instruction("BMI", 2, InsType::REL), //0x30
        Instruction("AND", 5, InsType::IZY), //0x31
        Instruction("KIL", 0, InsType::IMP), //0x32
        Instruction("RLA", 8, InsType::IZY), //0x33
        Instruction("NOP", 4, InsType::ZPX), //0x34
        Instruction("AND", 4, InsType::ZPX), //0x35
        Instruction("ROL", 6, InsType::ZPX), //0x36
        Instruction("RLA", 6, InsType::ZPX), //0x37
        Instruction("SEC", 2, InsType::IMP), //0x38
        Instruction("AND", 4, InsType::ABY), //0x39
        Instruction("NOP", 2, InsType::IMP), //0x3a
        Instruction("RLA", 7, InsType::ABY), //0x3b
        Instruction("NOP", 4, InsType::ABX), //0x3c
        Instruction("AND", 4, InsType::ABX), //0x3d
        Instruction("ROL", 7, InsType::ABX), //0x3e
        Instruction("RLA", 7, InsType::ABX), //0x3f
        Instruction("RTI", 6, InsType::IMP), //0x40
        Instruction("EOR", 6, InsType::IZX), //0x41
        Instruction("KIL", 0, InsType::IMP), //0x42
        Instruction("SRE", 8, InsType::IZX), //0x43
        Instruction("NOP", 3, InsType::ZP), //0x44
        Instruction("EOR", 3, InsType::ZP), //0x45
        Instruction("LSR", 5, InsType::ZP), //0x46
        Instruction("SRE", 5, InsType::ZP), //0x47
        Instruction("PHA", 3, InsType::IMP), //0x48
        Instruction("EOR", 2, InsType::IMM), //0x49
        Instruction("LSR", 2, InsType::IMP), //0x4a
        Instruction("ALR", 2, InsType::IMM), //0x4b
        Instruction("JMP", 3, InsType::ABS), //0x4c
        Instruction("EOR", 4, InsType::ABS), //0x4d
        Instruction("LSR", 6, InsType::ABS), //0x4e
        Instruction("SRE", 6, InsType::ABS), //0x4f
        Instruction("BVC", 2, InsType::REL), //0x50
        Instruction("EOR", 5, InsType::IZY), //0x51
        Instruction("KIL", 0, InsType::IMP), //0x52
        Instruction("SRE", 8, InsType::IZY), //0x53
        Instruction("NOP", 4, InsType::ZPX), //0x54
        Instruction("EOR", 4, InsType::ZPX), //0x55
        Instruction("LSR", 6, InsType::ZPX), //0x56
        Instruction("SRE", 6, InsType::ZPX), //0x57
        Instruction("CLI", 2, InsType::IMP), //0x58
        Instruction("EOR", 4, InsType::ABY), //0x59
        Instruction("NOP", 2, InsType::IMP), //0x5a
        Instruction("SRE", 7, InsType::ABY), //0x5b
        Instruction("NOP", 4, InsType::ABX), //0x5c
        Instruction("EOR", 4, InsType::ABX), //0x5d
        Instruction("LSR", 7, InsType::ABX), //0x5e
        Instruction("SRE", 7, InsType::ABX), //0x5f
        Instruction("RTS", 6, InsType::IMP), //0x60
        Instruction("ADC", 6, InsType::IZX), //0x61
        Instruction("KIL", 0, InsType::IMP), //0x62
        Instruction("RRA", 8, InsType::IZX), //0x63
        Instruction("NOP", 3, InsType::ZP), //0x64
        Instruction("ADC", 3, InsType::ZP), //0x65
        Instruction("ROR", 5, InsType::ZP), //0x66
        Instruction("RRA", 5, InsType::ZP), //0x67
        Instruction("PLA", 4, InsType::IMP), //0x68
        Instruction("ADC", 2, InsType::IMM), //0x69
        Instruction("ROR", 2, InsType::IMP), //0x6a
        Instruction("ARR", 2, InsType::IMM), //0x6b
        Instruction("JMP", 5, InsType::IND), //0x6c
        Instruction("ADC", 4, InsType::ABS), //0x6d
        Instruction("ROR", 6, InsType::ABS), //0x6e
        Instruction("RRA", 6, InsType::ABS), //0x6f
        Instruction("BVS", 2, InsType::REL), //0x70
        Instruction("ADC", 5, InsType::IZY), //0x71
        Instruction("KIL", 0, InsType::IMP), //0x72
        Instruction("RRA", 8, InsType::IZY), //0x73
        Instruction("NOP", 4, InsType::ZPX), //0x74
        Instruction("ADC", 4, InsType::ZPX), //0x75
        Instruction("ROR", 6, InsType::ZPX), //0x76
        Instruction("RRA", 6, InsType::ZPX), //0x77
        Instruction("SEI", 2, InsType::IMP), //0x78
        Instruction("ADC", 4, InsType::ABY), //0x79
        Instruction("NOP", 2, InsType::IMP), //0x7a
        Instruction("RRA", 7, InsType::ABY), //0x7b
        Instruction("NOP", 4, InsType::ABX), //0x7c
        Instruction("ADC", 4, InsType::ABX), //0x7d
        Instruction("ROR", 7, InsType::ABX), //0x7e
        Instruction("RRA", 7, InsType::ABX), //0x7f
        Instruction("NOP", 2, InsType::IMM), //0x80
        Instruction("STA", 6, InsType::IZX), //0x81
        Instruction("NOP", 2, InsType::IMM), //0x82
        Instruction("SAX", 6, InsType::IZX), //0x83
        Instruction("STY", 3, InsType::ZP), //0x84
        Instruction("STA", 3, InsType::ZP), //0x85
        Instruction("STX", 3, InsType::ZP), //0x86
        Instruction("SAX", 3, InsType::ZP), //0x87
        Instruction("DEY", 2, InsType::IMP), //0x88
        Instruction("NOP", 2, InsType::IMM), //0x89
        Instruction("TXA", 2, InsType::IMP), //0x8a
        Instruction("XAA", 2, InsType::IMM), //0x8b
        Instruction("STY", 4, InsType::ABS), //0x8c
        Instruction("STA", 4, InsType::ABS), //0x8d
        Instruction("STX", 4, InsType::ABS), //0x8e
        Instruction("SAX", 4, InsType::ABS), //0x8f
        Instruction("BCC", 2, InsType::REL), //0x90
        Instruction("STA", 6, InsType::IZY), //0x91
        Instruction("KIL", 0, InsType::IMP), //0x92
        Instruction("AHX", 6, InsType::IZY), //0x93
        Instruction("STY", 4, InsType::ZPX), //0x94
        Instruction("STA", 4, InsType::ZPX), //0x95
        Instruction("STX", 4, InsType::ZPY), //0x96
        Instruction("SAX", 4, InsType::ZPY), //0x97
        Instruction("TYA", 2, InsType::IMP), //0x98
        Instruction("STA", 5, InsType::ABY), //0x99
        Instruction("TXS", 2, InsType::IMP), //0x9a
        Instruction("TAS", 5, InsType::ABY), //0x9b
        Instruction("SHY", 5, InsType::ABX), //0x9c
        Instruction("STA", 5, InsType::ABX), //0x9d
        Instruction("SHX", 5, InsType::ABY), //0x9e
        Instruction("AHX", 5, InsType::ABY), //0x9f
        Instruction("LDY", 2, InsType::IMM), //0xa0
        Instruction("LDA", 6, InsType::IZX), //0xa1
        Instruction("LDX", 2, InsType::IMM), //0xa2
        Instruction("LAX", 6, InsType::IZX), //0xa3
        Instruction("LDY", 3, InsType::ZP), //0xa4
        Instruction("LDA", 3, InsType::ZP), //0xa5
        Instruction("LDX", 3, InsType::ZP), //0xa6
        Instruction("LAX", 3, InsType::ZP), //0xa7
        Instruction("TAY", 2, InsType::IMP), //0xa8
        Instruction("LDA", 2, InsType::IMM), //0xa9
        Instruction("TAX", 2, InsType::IMP), //0xaa
        Instruction("LAX", 2, InsType::IMM), //0xab
        Instruction("LDY", 4, InsType::ABS), //0xac
        Instruction("LDA", 4, InsType::ABS), //0xad
        Instruction("LDX", 4, InsType::ABS), //0xae
        Instruction("LAX", 4, InsType::ABS), //0xaf
        Instruction("BCS", 2, InsType::REL), //0xb0
        Instruction("LDA", 5, InsType::IZY), //0xb1
        Instruction("KIL", 0, InsType::IMP), //0xb2
        Instruction("LAX", 5, InsType::IZY), //0xb3
        Instruction("LDY", 4, InsType::ZPX), //0xb4
        Instruction("LDA", 4, InsType::ZPX), //0xb5
        Instruction("LDX", 4, InsType::ZPY), //0xb6
        Instruction("LAX", 4, InsType::ZPY), //0xb7
        Instruction("CLV", 2, InsType::IMP), //0xb8
        Instruction("LDA", 4, InsType::ABY), //0xb9
        Instruction("TSX", 2, InsType::IMP), //0xba
        Instruction("LAS", 4, InsType::ABY), //0xbb
        Instruction("LDY", 4, InsType::ABX), //0xbc
        Instruction("LDA", 4, InsType::ABX), //0xbd
        Instruction("LDX", 4, InsType::ABY), //0xbe
        Instruction("LAX", 4, InsType::ABY), //0xbf
        Instruction("CPY", 2, InsType::IMM), //0xc0
        Instruction("CMP", 6, InsType::IZX), //0xc1
        Instruction("NOP", 2, InsType::IMM), //0xc2
        Instruction("DCP", 8, InsType::IZX), //0xc3
        Instruction("CPY", 3, InsType::ZP), //0xc4
        Instruction("CMP", 3, InsType::ZP), //0xc5
        Instruction("DEC", 5, InsType::ZP), //0xc6
        Instruction("DCP", 5, InsType::ZP), //0xc7
        Instruction("INY", 2, InsType::IMP), //0xc8
        Instruction("CMP", 2, InsType::IMM), //0xc9
        Instruction("DEX", 2, InsType::IMP), //0xca
        Instruction("AXS", 2, InsType::IMM), //0xcb
        Instruction("CPY", 4, InsType::ABS), //0xcc
        Instruction("CMP", 4, InsType::ABS), //0xcd
        Instruction("DEC", 6, InsType::ABS), //0xce
        Instruction("DCP", 6, InsType::ABS), //0xcf
        Instruction("BNE", 2, InsType::REL), //0xd0
        Instruction("CMP", 5, InsType::IZY), //0xd1
        Instruction("KIL", 0, InsType::IMP), //0xd2
        Instruction("DCP", 8, InsType::IZY), //0xd3
        Instruction("NOP", 4, InsType::ZPX), //0xd4
        Instruction("CMP", 4, InsType::ZPX), //0xd5
        Instruction("DEC", 6, InsType::ZPX), //0xd6
        Instruction("DCP", 6, InsType::ZPX), //0xd7
        Instruction("CLD", 2, InsType::IMP), //0xd8
        Instruction("CMP", 4, InsType::ABY), //0xd9
        Instruction("NOP", 2, InsType::IMP), //0xda
        Instruction("DCP", 7, InsType::ABY), //0xdb
        Instruction("NOP", 4, InsType::ABX), //0xdc
        Instruction("CMP", 4, InsType::ABX), //0xdd
        Instruction("DEC", 7, InsType::ABX), //0xde
        Instruction("DCP", 7, InsType::ABX), //0xdf
        Instruction("CPX", 2, InsType::IMM), //0xe0
        Instruction("SBC", 6, InsType::IZX), //0xe1
        Instruction("NOP", 2, InsType::IMM), //0xe2
        Instruction("ISC", 8, InsType::IZX), //0xe3
        Instruction("CPX", 3, InsType::ZP), //0xe4
        Instruction("SBC", 3, InsType::ZP), //0xe5
        Instruction("INC", 5, InsType::ZP), //0xe6
        Instruction("ISC", 5, InsType::ZP), //0xe7
        Instruction("INX", 2, InsType::IMP), //0xe8
        Instruction("SBC", 2, InsType::IMM), //0xe9
        Instruction("NOP", 2, InsType::IMP), //0xea
        Instruction("SBC", 2, InsType::IMM), //0xeb
        Instruction("CPX", 4, InsType::ABS), //0xec
        Instruction("SBC", 4, InsType::ABS), //0xed
        Instruction("INC", 6, InsType::ABS), //0xee
        Instruction("ISC", 6, InsType::ABS), //0xef
        Instruction("BEQ", 2, InsType::REL), //0xf0
        Instruction("SBC", 5, InsType::IZY), //0xf1
        Instruction("KIL", 0, InsType::IMP), //0xf2
        Instruction("ISC", 8, InsType::IZY), //0xf3
        Instruction("NOP", 4, InsType::ZPX), //0xf4
        Instruction("SBC", 4, InsType::ZPX), //0xf5
        Instruction("INC", 6, InsType::ZPX), //0xf6
        Instruction("ISC", 6, InsType::ZPX), //0xf7
        Instruction("SED", 2, InsType::IMP), //0xf8
        Instruction("SBC", 4, InsType::ABY), //0xf9
        Instruction("NOP", 2, InsType::IMP), //0xfa
        Instruction("ISC", 7, InsType::ABY), //0xfb
        Instruction("NOP", 4, InsType::ABX), //0xfc
        Instruction("SBC", 4, InsType::ABX), //0xfd
        Instruction("INC", 7, InsType::ABX), //0xfe
        Instruction("ISC", 7, InsType::ABX), //0xff
};