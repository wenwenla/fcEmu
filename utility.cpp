//
// Created by DELL on 2019/8/29.
//

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "utility.h"

inline int twoBytes(Byte low, Byte high) {
    return high * 256 + low;
}

std::string decode(const std::vector<Byte> &ins) {
    constexpr int IMM = 0, ZP = 1, ZPX = 2, ZPY = 3, IZX = 4, IZY = 5, ABS = 6,
            ABX = 7, ABY = 8, IND = 9, REL = 10, NONE = 11;
    constexpr char fmt[][20] = {
            "%s #$%02x", //imm 0
            "%s $%02x", //zp 1
            "%s $%02x, X", //zpx 2
            "%s $%02x, Y", //zpy 3
            "%s ($%02x, X)", //izx 4
            "%s ($%02x), Y", //izy 5
            "%s $%04x", //abs 6
            "%s $%04x, X", //abx 7
            "%s $%04x, Y", //aby 8
            "%s ($%04x)", //ind 9
            "%s $%04x", //rel 10
            "%s", //NONE
    };
    char buf[32];
    switch (ins[0]) {
        case 0x00:
            sprintf(buf, fmt[NONE], "BRK");
            break;
        case 0x01:
            sprintf(buf, fmt[IZX], "ORA", ins[1]);
            break;
        case 0x02:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x03:
            sprintf(buf, fmt[IZX], "SLO", ins[1]);
            break;
        case 0x04:
            sprintf(buf, fmt[ZP], "NOP", ins[1]);
            break;
        case 0x05:
            sprintf(buf, fmt[ZP], "ORA", ins[1]);
            break;
        case 0x06:
            sprintf(buf, fmt[ZP], "ASL", ins[1]);
            break;
        case 0x07:
            sprintf(buf, fmt[ZP], "SLO", ins[1]);
            break;
        case 0x08:
            sprintf(buf, fmt[NONE], "PHP");
            break;
        case 0x09:
            sprintf(buf, fmt[IMM], "ORA", ins[1]);
            break;
        case 0x0a:
            sprintf(buf, fmt[NONE], "ASL");
            break;
        case 0x0b:
            sprintf(buf, fmt[IMM], "ANC", ins[1]);
            break;
        case 0x0c:
            sprintf(buf, fmt[ABS], "NOP", twoBytes(ins[1], ins[2]));
            break;
        case 0x0d:
            sprintf(buf, fmt[ABS], "ORA", twoBytes(ins[1], ins[2]));
            break;
        case 0x0e:
            sprintf(buf, fmt[ABS], "ASL", twoBytes(ins[1], ins[2]));
            break;
        case 0x0f:
            sprintf(buf, fmt[ABS], "SLO", twoBytes(ins[1], ins[2]));
            break;
        case 0x10:
            sprintf(buf, fmt[REL], "BPL", twoBytes(ins[1], ins[2]));
            break;
        case 0x11:
            sprintf(buf, fmt[IZY], "ORA", ins[1]);
            break;
        case 0x12:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x13:
            sprintf(buf, fmt[IZY], "SLO", ins[1]);
            break;
        case 0x14:
            sprintf(buf, fmt[ZP], "NOP", ins[1]);
            break;
        case 0x15:
            sprintf(buf, fmt[ZPX], "ORA", ins[1]);
            break;
        case 0x16:
            sprintf(buf, fmt[ZPX], "ASL", ins[1]);
            break;
        case 0x17:
            sprintf(buf, fmt[ZPX], "SLO", ins[1]);
            break;
        case 0x18:
            sprintf(buf, fmt[NONE], "CLC");
            break;
        case 0x19:
            sprintf(buf, fmt[ABY], "ORA", twoBytes(ins[1], ins[2]));
            break;
        case 0x1a:
            sprintf(buf, fmt[NONE], "NOP");
            break;
        case 0x1b:
            sprintf(buf, fmt[ABY], "SLO", twoBytes(ins[1], ins[2]));
            break;
        case 0x1c:
            sprintf(buf, fmt[ABX], "NOP", twoBytes(ins[1], ins[2]));
            break;
        case 0x1d:
            sprintf(buf, fmt[ABX], "ORA", twoBytes(ins[1], ins[2]));
            break;
        case 0x1e:
            sprintf(buf, fmt[ABX], "ASL", twoBytes(ins[1], ins[2]));
            break;
        case 0x1f:
            sprintf(buf, fmt[ABX], "SLO", twoBytes(ins[1], ins[2]));
            break;
        case 0x20:
            sprintf(buf, fmt[ABS], "JSR", twoBytes(ins[1], ins[2]));
            break;
        case 0x21:
            sprintf(buf, fmt[IZX], "AND", ins[1]);
            break;
        case 0x22:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x23:
            sprintf(buf, fmt[IZX], "RLA", ins[1]);
            break;
        case 0x24:
            sprintf(buf, fmt[ZP], "BIT", ins[1]);
            break;
        case 0x25:
            sprintf(buf, fmt[ZP], "AND", ins[1]);
            break;
        case 0x26:
            sprintf(buf, fmt[ZP], "ROL", ins[1]);
            break;
        case 0x27:
            sprintf(buf, fmt[ZP], "RLA", ins[1]);
            break;
        case 0x28:
            sprintf(buf, fmt[NONE], "PLP");
            break;
        case 0x29:
            sprintf(buf, fmt[IMM], "AND", ins[1]);
            break;
        case 0x2a:
            sprintf(buf, fmt[NONE], "ROL");
            break;
        case 0x2b:
            sprintf(buf, fmt[IMM], "ANC", ins[1]);
            break;
        case 0x2c:
            sprintf(buf, fmt[ABS], "BIT", twoBytes(ins[1], ins[2]));
            break;
        case 0x2d:
            sprintf(buf, fmt[ABS], "AND", twoBytes(ins[1], ins[2]));
            break;
        case 0x2e:
            sprintf(buf, fmt[ABS], "ROL", twoBytes(ins[1], ins[2]));
            break;
        case 0x2f:
            sprintf(buf, fmt[ABS], "RLA", twoBytes(ins[1], ins[2]));
            break;
        case 0x30:
            sprintf(buf, fmt[REL], "BMI", twoBytes(ins[1], ins[2]));
            break;
        case 0x31:
            sprintf(buf, fmt[IZY], "AND", ins[1]);
            break;
        case 0x32:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x33:
            sprintf(buf, fmt[IZY], "RLA", ins[1]);
            break;
        case 0x34:
            sprintf(buf, fmt[ZPX], "NOP", ins[1]);
            break;
        case 0x35:
            sprintf(buf, fmt[ZPX], "AND", ins[1]);
            break;
        case 0x36:
            sprintf(buf, fmt[ZPX], "ROL", ins[1]);
            break;
        case 0x37:
            sprintf(buf, fmt[ZPX], "RLA", ins[1]);
            break;
        case 0x38:
            sprintf(buf, fmt[NONE], "SEC");
            break;
        case 0x39:
            sprintf(buf, fmt[ABY], "AND", twoBytes(ins[1], ins[2]));
            break;
        case 0x3a:
            sprintf(buf, fmt[NONE], "NOP");
            break;
        case 0x3b:
            sprintf(buf, fmt[ABY], "RLA", twoBytes(ins[1], ins[2]));
            break;
        case 0x3c:
            sprintf(buf, fmt[ABX], "NOP", twoBytes(ins[1], ins[2]));
            break;
        case 0x3d:
            sprintf(buf, fmt[ABX], "AND", twoBytes(ins[1], ins[2]));
            break;
        case 0x3e:
            sprintf(buf, fmt[ABX], "ROL", twoBytes(ins[1], ins[2]));
            break;
        case 0x3f:
            sprintf(buf, fmt[ABX], "RLA", twoBytes(ins[1], ins[2]));
            break;
        case 0x40: //7
            sprintf(buf, fmt[NONE], "RTI");
            break;
        case 0x41:
            sprintf(buf, fmt[IZX], "EOR", ins[1]);
            break;
        case 0x42:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x43:
            sprintf(buf, fmt[IZX], "SRE", ins[1]);
            break;
        case 0x44:
            sprintf(buf, fmt[ZP], "NOP", ins[1]);
            break;
        case 0x45:
            sprintf(buf, fmt[ZP], "EOR", ins[1]);
            break;
        case 0x46:
            sprintf(buf, fmt[ZP], "LSR", ins[1]);
            break;
        case 0x47:
            sprintf(buf, fmt[ZP], "SRE", ins[1]);
            break;
        case 0x48:
            sprintf(buf, fmt[NONE], "PHA");
            break;
        case 0x49:
            sprintf(buf, fmt[IMM], "EOR", ins[1]);
            break;
        case 0x4a:
            sprintf(buf, fmt[NONE], "LSR");
            break;
        case 0x4b:
            sprintf(buf, fmt[IMM], "ALR", ins[1]);
            break;
        case 0x4c:
            sprintf(buf, fmt[ABS], "JMP", twoBytes(ins[1], ins[2]));
            break;
        case 0x4d:
            sprintf(buf, fmt[ABS], "EOR", twoBytes(ins[1], ins[2]));
            break;
        case 0x4e:
            sprintf(buf, fmt[ABS], "LSR", twoBytes(ins[1], ins[2]));
            break;
        case 0x4f:
            sprintf(buf, fmt[ABS], "SRE", twoBytes(ins[1], ins[2]));
            break;
        case 0x50:
            sprintf(buf, fmt[REL], "BVC", twoBytes(ins[1], ins[2]));
            break;
        case 0x51:
            sprintf(buf, fmt[IZY], "EOR", ins[1]);
            break;
        case 0x52:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x53:
            sprintf(buf, fmt[IZY], "SRE", ins[1]);
            break;
        case 0x54:
            sprintf(buf, fmt[ZPX], "NOP", ins[1]);
            break;
        case 0x55:
            sprintf(buf, fmt[ZPX], "EOR", ins[1]);
            break;
        case 0x56:
            sprintf(buf, fmt[ZPX], "LSR", ins[1]);
            break;
        case 0x57:
            sprintf(buf, fmt[ZPX], "SRE", ins[1]);
            break;
        case 0x58:
            sprintf(buf, fmt[NONE], "CLI");
            break;
        case 0x59:
            sprintf(buf, fmt[ABY], "EOR", twoBytes(ins[1], ins[2]));
            break;
        case 0x5a:
            sprintf(buf, fmt[NONE], "NOP");
            break;
        case 0x5b:
            sprintf(buf, fmt[ABY], "SRE", twoBytes(ins[1], ins[2]));
            break;
        case 0x5c:
            sprintf(buf, fmt[ABX], "NOP", twoBytes(ins[1], ins[2]));
            break;
        case 0x5d:
            sprintf(buf, fmt[ABX], "EOR", twoBytes(ins[1], ins[2]));
            break;
        case 0x5e:
            sprintf(buf, fmt[ABX], "LSR", twoBytes(ins[1], ins[2]));
            break;
        case 0x5f:
            sprintf(buf, fmt[ABX], "SRE", twoBytes(ins[1], ins[2]));
            break;
        case 0x60:
            sprintf(buf, fmt[NONE], "RTS");
            break;
        case 0x61:
            sprintf(buf, fmt[IZX], "ADC", ins[1]);
            break;
        case 0x62:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x63:
            sprintf(buf, fmt[IZX], "RRA", ins[1]);
            break;
        case 0x64:
            sprintf(buf, fmt[ZP], "NOP", ins[1]);
            break;
        case 0x65:
            sprintf(buf, fmt[ZP], "ADC", ins[1]);
            break;
        case 0x66:
            sprintf(buf, fmt[ZP], "ROR", ins[1]);
            break;
        case 0x67:
            sprintf(buf, fmt[ZP], "RRA", ins[1]);
            break;
        case 0x68:
            sprintf(buf, fmt[NONE], "PLA");
            break;
        case 0x69:
            sprintf(buf, fmt[IMM], "ADC", ins[1]);
            break;
        case 0x6a:
            sprintf(buf, fmt[NONE], "ROR");
            break;
        case 0x6b:
            sprintf(buf, fmt[IMM], "ARR", ins[1]);
            break;
        case 0x6c:
            sprintf(buf, fmt[IND], "JMP", twoBytes(ins[1], ins[2]));
            break;
        case 0x6d:
            sprintf(buf, fmt[ABS], "ADC", twoBytes(ins[1], ins[2]));
            break;
        case 0x6e:
            sprintf(buf, fmt[ABS], "ROR", twoBytes(ins[1], ins[2]));
            break;
        case 0x6f:
            sprintf(buf, fmt[ABS], "RRA", twoBytes(ins[1], ins[2]));
            break;
        case 0x70:
            sprintf(buf, fmt[REL], "BVS", twoBytes(ins[1], ins[2]));
            break;
        case 0x71:
            sprintf(buf, fmt[IZY], "ADC", ins[1]);
            break;
        case 0x72:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x73:
            sprintf(buf, fmt[IZY], "RRA", ins[1]);
            break;
        case 0x74:
            sprintf(buf, fmt[ZPX], "NOP", ins[1]);
            break;
        case 0x75:
            sprintf(buf, fmt[ZPX], "ADC", ins[1]);
            break;
        case 0x76:
            sprintf(buf, fmt[ZPX], "ROR", ins[1]);
            break;
        case 0x77:
            sprintf(buf, fmt[ZPX], "RRA", ins[1]);
            break;
        case 0x78:
            sprintf(buf, fmt[NONE], "SEI");
            break;
        case 0x79:
            sprintf(buf, fmt[ABY], "ADC", twoBytes(ins[1], ins[2]));
            break;
        case 0x7a:
            sprintf(buf, fmt[NONE], "NOP");
            break;
        case 0x7b:
            sprintf(buf, fmt[ABY], "RRA", twoBytes(ins[1], ins[2]));
            break;
        case 0x7c:
            sprintf(buf, fmt[ABX], "NOP", twoBytes(ins[1], ins[2]));
            break;
        case 0x7d:
            sprintf(buf, fmt[ABX], "ADC", twoBytes(ins[1], ins[2]));
            break;
        case 0x7e:
            sprintf(buf, fmt[ABX], "ROR", twoBytes(ins[1], ins[2]));
            break;
        case 0x7f:
            sprintf(buf, fmt[ABX], "RRA", twoBytes(ins[1], ins[2]));
            break;
        case 0x80:
            sprintf(buf, fmt[IMM], "NOP", ins[1]);
            break;
        case 0x81:
            sprintf(buf, fmt[IZX], "STA", ins[1]);
            break;
        case 0x82:
            sprintf(buf, fmt[IMM], "NOP", ins[1]);
            break;
        case 0x83:
            sprintf(buf, fmt[IZX], "SAX", ins[1]);
            break;
        case 0x84:
            sprintf(buf, fmt[ZP], "STY", ins[1]);
            break;
        case 0x85:
            sprintf(buf, fmt[ZP], "STA", ins[1]);
            break;
        case 0x86:
            sprintf(buf, fmt[ZP], "STX", ins[1]);
            break;
        case 0x87:
            sprintf(buf, fmt[ZP], "SAX", ins[1]);
            break;
        case 0x88:
            sprintf(buf, fmt[NONE], "DEY");
            break;
        case 0x89:
            sprintf(buf, fmt[IMM], "NOP", ins[1]);
            break;
        case 0x8a:
            sprintf(buf, fmt[NONE], "TXA");
            break;
        case 0x8b:
            sprintf(buf, fmt[IMM], "XAA", ins[1]);
            break;
        case 0x8c:
            sprintf(buf, fmt[ABS], "STY", twoBytes(ins[1], ins[2]));
            break;
        case 0x8d:
            sprintf(buf, fmt[ABS], "STA", twoBytes(ins[1], ins[2]));
            break;
        case 0x8e:
            sprintf(buf, fmt[ABS], "STX", twoBytes(ins[1], ins[2]));
            break;
        case 0x8f:
            sprintf(buf, fmt[ABS], "SAX", twoBytes(ins[1], ins[2]));
            break;
        case 0x90:
            sprintf(buf, fmt[REL], "BCC", twoBytes(ins[1], ins[2]));
            break;
        case 0x91:
            sprintf(buf, fmt[IZY], "STA", ins[1]);
            break;
        case 0x92:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0x93:
            sprintf(buf, fmt[IZY], "AHX", ins[1]);
            break;
        case 0x94:
            sprintf(buf, fmt[ZPX], "STY", ins[1]);
            break;
        case 0x95:
            sprintf(buf, fmt[ZPX], "STA", ins[1]);
            break;
        case 0x96:
            sprintf(buf, fmt[ZPY], "STX", ins[1]);
            break;
        case 0x97:
            sprintf(buf, fmt[ZPY], "SAX", ins[1]);
            break;
        case 0x98:
            sprintf(buf, fmt[NONE], "TYA");
            break;
        case 0x99:
            sprintf(buf, fmt[ABY], "STA", twoBytes(ins[1], ins[2]));
            break;
        case 0x9a:
            sprintf(buf, fmt[NONE], "TXS");
            break;
        case 0x9b:
            sprintf(buf, fmt[ABY], "TAS", twoBytes(ins[1], ins[2]));
            break;
        case 0x9c:
            sprintf(buf, fmt[ABX], "SHY", twoBytes(ins[1], ins[2]));
            break;
        case 0x9d:
            sprintf(buf, fmt[ABX], "STA", twoBytes(ins[1], ins[2]));
            break;
        case 0x9e:
            sprintf(buf, fmt[ABY], "SHX", twoBytes(ins[1], ins[2]));
            break;
        case 0x9f:
            sprintf(buf, fmt[ABY], "AHX", twoBytes(ins[1], ins[2]));
            break;
        case 0xa0:
            sprintf(buf, fmt[IMM], "LDY", ins[1]);
            break;
        case 0xa1:
            sprintf(buf, fmt[IZX], "LDA", ins[1]);
            break;
        case 0xa2:
            sprintf(buf, fmt[IMM], "LDX", ins[1]);
            break;
        case 0xa3:
            sprintf(buf, fmt[IZX], "LAX", ins[1]);
            break;
        case 0xa4:
            sprintf(buf, fmt[ZP], "LDY", ins[1]);
            break;
        case 0xa5:
            sprintf(buf, fmt[ZP], "LDA", ins[1]);
            break;
        case 0xa6:
            sprintf(buf, fmt[ZP], "LDX", ins[1]);
            break;
        case 0xa7:
            sprintf(buf, fmt[ZP], "LAX", ins[1]);
            break;
        case 0xa8:
            sprintf(buf, fmt[NONE], "TAY");
            break;
        case 0xa9:
            sprintf(buf, fmt[IMM], "LDA", ins[1]);
            break;
        case 0xaa:
            sprintf(buf, fmt[NONE], "TAX");
            break;
        case 0xab:
            sprintf(buf, fmt[IMM], "LAX", ins[1]);
            break;
        case 0xac:
            sprintf(buf, fmt[ABS], "LDY", twoBytes(ins[1], ins[2]));
            break;
        case 0xad:
            sprintf(buf, fmt[ABS], "LDA", twoBytes(ins[1], ins[2]));
            break;
        case 0xae:
            sprintf(buf, fmt[ABS], "LDX", twoBytes(ins[1], ins[2]));
            break;
        case 0xaf:
            sprintf(buf, fmt[ABS], "LAX", twoBytes(ins[1], ins[2]));
            break;
        case 0xb0:
            sprintf(buf, fmt[REL], "BCC", twoBytes(ins[1], ins[2]));
            break;
        case 0xb1:
            sprintf(buf, fmt[IZY], "LDA", ins[1]);
            break;
        case 0xb2:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0xb3:
            sprintf(buf, fmt[IZY], "LAX", ins[1]);
            break;
        case 0xb4:
            sprintf(buf, fmt[ZPX], "LDY", ins[1]);
            break;
        case 0xb5:
            sprintf(buf, fmt[ZPX], "LDA", ins[1]);
            break;
        case 0xb6:
            sprintf(buf, fmt[ZPY], "LDX", ins[1]);
            break;
        case 0xb7:
            sprintf(buf, fmt[ZPY], "LAX", ins[1]);
            break;
        case 0xb8:
            sprintf(buf, fmt[NONE], "CLV", ins[1]);
            break;
        case 0xb9:
            sprintf(buf, fmt[ABY], "LDA", twoBytes(ins[1], ins[2]));
            break;
        case 0xba:
            sprintf(buf, fmt[NONE], "TSX");
            break;
        case 0xbb:
            sprintf(buf, fmt[ABY], "LAS", twoBytes(ins[1], ins[2]));
            break;
        case 0xbc:
            sprintf(buf, fmt[ABX], "LDY", twoBytes(ins[1], ins[2]));
            break;
        case 0xbd:
            sprintf(buf, fmt[ABX], "LDA", twoBytes(ins[1], ins[2]));
            break;
        case 0xbe:
            sprintf(buf, fmt[ABY], "LDX", twoBytes(ins[1], ins[2]));
            break;
        case 0xbf:
            sprintf(buf, fmt[ABY], "LAX", twoBytes(ins[1], ins[2]));
            break;
        case 0xc0:
            sprintf(buf, fmt[IMM], "CPY", ins[1]);
            break;
        case 0xc1:
            sprintf(buf, fmt[IZX], "CMP", ins[1]);
            break;
        case 0xc2:
            sprintf(buf, fmt[IMM], "NOP", ins[1]);
            break;
        case 0xc3:
            sprintf(buf, fmt[IZX], "DCP", ins[1]);
            break;
        case 0xc4:
            sprintf(buf, fmt[ZP], "CPY", ins[1]);
            break;
        case 0xc5:
            sprintf(buf, fmt[ZP], "CMP", ins[1]);
            break;
        case 0xc6:
            sprintf(buf, fmt[ZP], "DEC", ins[1]);
            break;
        case 0xc7:
            sprintf(buf, fmt[ZP], "DCP", ins[1]);
            break;
        case 0xc8:
            sprintf(buf, fmt[NONE], "INY");
            break;
        case 0xc9:
            sprintf(buf, fmt[IMM], "CMP", ins[1]);
            break;
        case 0xca:
            sprintf(buf, fmt[NONE], "DEX");
            break;
        case 0xcb:
            sprintf(buf, fmt[IMM], "AXS", ins[1]);
            break;
        case 0xcc:
            sprintf(buf, fmt[ABS], "CPY", twoBytes(ins[1], ins[2]));
            break;
        case 0xcd:
            sprintf(buf, fmt[ABX], "CMP", twoBytes(ins[1], ins[2]));
            break;
        case 0xce:
            sprintf(buf, fmt[ABS], "DEC", twoBytes(ins[1], ins[2]));
            break;
        case 0xcf:
            sprintf(buf, fmt[ABS], "DCP", twoBytes(ins[1], ins[2]));
            break;
        case 0xd0:
            sprintf(buf, fmt[REL], "BNE", twoBytes(ins[1], ins[2]));
            break;
        case 0xd1:
            sprintf(buf, fmt[IZY], "CMP", ins[1]);
            break;
        case 0xd2:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0xd3:
            sprintf(buf, fmt[IZY], "DCP", ins[1]);
            break;
        case 0xd4:
            sprintf(buf, fmt[ZPX], "NOP", ins[1]);
            break;
        case 0xd5:
            sprintf(buf, fmt[ZPX], "CMP", ins[1]);
            break;
        case 0xd6:
            sprintf(buf, fmt[ZPX], "DEC", ins[1]);
            break;
        case 0xd7:
            sprintf(buf, fmt[ZPX], "DCP", ins[1]);
            break;
        case 0xd8:
            sprintf(buf, fmt[NONE], "CLD");
            break;
        case 0xd9:
            sprintf(buf, fmt[ABY], "CMP", twoBytes(ins[1], ins[2]));
            break;
        case 0xda:
            sprintf(buf, fmt[NONE], "NOP");
            break;
        case 0xdb:
            sprintf(buf, fmt[ABY], "DCP", twoBytes(ins[1], ins[2]));
            break;
        case 0xdc:
            sprintf(buf, fmt[ABX], "NOP", twoBytes(ins[1], ins[2]));
            break;
        case 0xdd:
            sprintf(buf, fmt[ABX], "CMP", twoBytes(ins[1], ins[2]));
            break;
        case 0xde:
            sprintf(buf, fmt[ABX], "DEC", twoBytes(ins[1], ins[2]));
            break;
        case 0xdf:
            sprintf(buf, fmt[ABX], "DCP", twoBytes(ins[1], ins[2]));
            break;
        case 0xe0:
            sprintf(buf, fmt[IMM], "CPX", ins[1]);
            break;
        case 0xe1:
            sprintf(buf, fmt[IZX], "SBC", ins[1]);
            break;
        case 0xe2:
            sprintf(buf, fmt[IMM], "NOP", ins[1]);
            break;
        case 0xe3:
            sprintf(buf, fmt[IZX], "ISC", ins[1]);
            break;
        case 0xe4:
            sprintf(buf, fmt[ZP], "CPX", ins[1]);
            break;
        case 0xe5:
            sprintf(buf, fmt[ZP], "SBC", ins[1]);
            break;
        case 0xe6:
            sprintf(buf, fmt[ZP], "INC", ins[1]);
            break;
        case 0xe7:
            sprintf(buf, fmt[ZP], "ISC", ins[1]);
            break;
        case 0xe8:
            sprintf(buf, fmt[NONE], "INX");
            break;
        case 0xe9:
            sprintf(buf, fmt[IMM], "SBC", ins[1]);
            break;
        case 0xea:
            sprintf(buf, fmt[NONE], "NOP");
            break;
        case 0xeb:
            sprintf(buf, fmt[IMM], "SBC", ins[1]);
            break;
        case 0xec:
            sprintf(buf, fmt[ABS], "CPX", twoBytes(ins[1], ins[2]));
            break;
        case 0xed:
            sprintf(buf, fmt[ABS], "SBC", twoBytes(ins[1], ins[2]));
            break;
        case 0xee:
            sprintf(buf, fmt[ABS], "INC", twoBytes(ins[1], ins[2]));
            break;
        case 0xef:
            sprintf(buf, fmt[ABS], "ISC", twoBytes(ins[1], ins[2]));
            break;
        case 0xf0:
            sprintf(buf, fmt[REL], "BEQ", twoBytes(ins[1], ins[2]));
            break;
        case 0xf1:
            sprintf(buf, fmt[IZY], "SBC", ins[1]);
            break;
        case 0xf2:
            sprintf(buf, fmt[NONE], "STP");
            break;
        case 0xf3:
            sprintf(buf, fmt[IZY], "ISC", ins[1]);
            break;
        case 0xf4:
            sprintf(buf, fmt[ZPX], "NOP", ins[1]);
            break;
        case 0xf5:
            sprintf(buf, fmt[ZPX], "SBC", ins[1]);
            break;
        case 0xf6:
            sprintf(buf, fmt[ZPX], "INC", ins[1]);
            break;
        case 0xf7:
            sprintf(buf, fmt[ZPX], "ISC", ins[1]);
            break;
        case 0xf8:
            sprintf(buf, fmt[NONE], "SED");
            break;
        case 0xf9:
            sprintf(buf, fmt[ABY], "SBC", twoBytes(ins[1], ins[2]));
            break;
        case 0xfa:
            sprintf(buf, fmt[NONE], "NOP");
            break;
        case 0xfb:
            sprintf(buf, fmt[ABY], "ISC", twoBytes(ins[1], ins[2]));
            break;
        case 0xfc:
            sprintf(buf, fmt[ABX], "NOP", twoBytes(ins[1], ins[2]));
            break;
        case 0xfd:
            sprintf(buf, fmt[ABX], "SBC", twoBytes(ins[1], ins[2]));
            break;
        case 0xfe:
            sprintf(buf, fmt[ABX], "INC", twoBytes(ins[1], ins[2]));
            break;
        case 0xff:
            sprintf(buf, fmt[ABX], "ISC", twoBytes(ins[1], ins[2]));
    }
    return buf;
}