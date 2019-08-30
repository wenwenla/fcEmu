//
// Created by DELL on 2019/8/29.
//

#include "Cpu.h"
#include "../utility.h"
#include "Instruction.h"
#include <iostream>
#include <cassert>


std::string Cpu::decode(Byte op) const {
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
            "%s", //imp 11
    };
    auto ins = Instruction::INS[op];
    char buf[32] = {0};
    auto data = _mem.read(_pc, ins.length());
    sprintf(buf, fmt[static_cast<int>(ins.getType())], ins.getName().c_str(), _get_data(ins, data));
    return buf;
}

int Cpu::_get_data(const Instruction &ins, const std::vector<Byte> &data) const {
    int result = -1;
    switch (ins.getType()) {
        case InsType::IMM:
            result = data[1];
            break;
        case InsType::ZP:
            result = data[1];
            break;
        case InsType::ZPX:
            assert(!"ZPX");
            break;
        case InsType::ZPY:
            assert(!"ZPY");
            break;
        case InsType::IZX:
            assert(!"IZX");
            break;
        case InsType::IZY:
            assert(!"IZY");
            break;
        case InsType::ABS:
            result = data[1] + data[2] * 256;
            break;
        case InsType::ABX:
            assert(!"ABX");
            break;
        case InsType::ABY:
            assert(!"ABY");
            break;
        case InsType::IND:
            assert(!"IND");
            break;
        case InsType::REL:
            result = _pc + (int)data[1] + ins.length();
            break;
        case InsType::IMP:
            result = 0;
            printf("IMP\n");
            break;
    }
    return result;
}

Cpu::Cpu(Memory& memory) : _reg_a(0), _reg_x(0), _reg_y(0), _sp(0xfd), _now_cycle(7), _mem(memory) {
    _pc = 0xc000; //test
    //_pc = _mem.read(0xfffc) + _mem.read(0xfffd) * 256;
    _flag.setIntDisable(true);
}

int Cpu::run() {
    log();
    Byte op = _mem.read(_pc);
    auto ins = Instruction::INS[op];
    int len = ins.length();
    auto data = _mem.read(_pc, len);
    switch (op) {
        case 0x4c:
            _jmp(ins, data);
            break;
        case 0xa2:
            _ldx(ins, data);
            break;
        case 0x86:
            _stx(ins, data);
            break;
        case 0x20:
            _jsr(ins, data);
            break;
        case 0xea:
            _nop(ins, data);
            break;
        case 0x38:
            _sec(ins, data);
            break;
        case 0xb0:
            _bcs(ins, data);
            break;
        case 0x18:
            _clc(ins, data);
            break;
        case 0x90:
            _bcc(ins, data);
            break;
        case 0xa9:
            _lda(ins, data);
            break;
        case 0xf0:
            _beq(ins, data);
            break;
        case 0xd0:
            _bne(ins, data);
            break;
        case 0x85:
            _sta(ins, data);
            break;
        case 0x24:
            _bit(ins, data);
            break;
        case 0x70:
            _bvs(ins, data);
            break;
        case 0x50:
            _bvc(ins, data);
            break;
        case 0x10:
            _bpl(ins, data);
            break;
        case 0x60:
            _rts(ins, data);
            break;
        case 0x78:
            _sei(ins, data);
            break;
        case 0xf8:
            _sed(ins, data);
            break;
        case 0x08:
            _php(ins, data);
            break;
        case 0x68:
            _pla(ins, data);
            break;
        case 0x29:
            _and(ins, data);
            break;
        case 0xc9:
            _cmp(ins, data);
            break;
        default:
            printf("UNKNOWN OP $%02x!", op);
            exit(-1);
    }
    return 0;
}

void Cpu::log() const {
    Byte op = _mem.read(_pc);
    auto ins = Instruction::INS[op];
    int len = ins.length();
    auto ins_data = _mem.read(_pc, len);
    auto msg = decode(op);
    char data[32] = {0};
    if(len == 1) {
        sprintf(data, "%02x", ins_data[0]);
    } else if(len == 2) {
        sprintf(data, "%02x %02x", ins_data[0], ins_data[1]);
    } else if(len == 3) {
        sprintf(data, "%02x %02x %02x", ins_data[0], ins_data[1], ins_data[2]);
    } else {
        assert(!"LEN ERROR");
    }
    printf("$%04x: [%s](%s) {A: $%02x X: $%02x Y: $%02x P: $%02x SP: $%02x CYC: %d}\n",
            _pc, data, msg.c_str(), _reg_a, _reg_x, _reg_y, _flag.toInt(), _sp, _now_cycle);
}

void Cpu::_stack_push(Byte byte) {
    _mem.write(_sp--, byte);
}

Byte Cpu::_stack_pop() {
    return _mem.read(++_sp);
}


void Cpu::_jmp(const Instruction& ins, const std::vector<Byte>& data) {
    assert(ins.getName() == "JMP");
    _pc = _get_data(ins, data);
    _now_cycle += ins.getCycle();
}


void Cpu::_ldx(const Instruction &ins, const std::vector<Byte>& data) {
    assert(ins.getName() == "LDX");
    _reg_x = _get_data(ins, data);
    _flag.setZero(_reg_x == 0);
    _flag.setNegative((_reg_x >> 7 & 1) == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_stx(const Instruction& ins, const std::vector<Byte>& data) {
    assert(ins.getName() == "STX");
    int d = _get_data(ins, data);
    _mem.write(d, _reg_x);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_jsr(const Instruction& ins, const std::vector<Byte>& data) {
    assert(_pc == combine(low(_pc), high(_pc)));
    assert(ins.getName() == "JSR");
    unsigned nxt = _pc + ins.length();
    _stack_push(low(nxt));
    _stack_push(high(nxt));
    _pc = _get_data(ins, data);
    _now_cycle += ins.getCycle();
}

void Cpu::_nop(const Instruction& ins, const std::vector<Byte>& data) {
    assert(ins.getName() == "NOP");
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_sec(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "SEC");
    _flag.setCarry(true);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_bcs(const Instruction& ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BCS");
    if(_flag.getCarry()) {
        _pc = _get_data(ins, data);
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_clc(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "CLC");
    _flag.setCarry(false);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_bcc(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BCC");
    if(!_flag.getCarry()) {
        _pc = _get_data(ins, data);
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_lda(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "LDA");
    _reg_a = _get_data(ins, data);
    _flag.setZero(_reg_a == 0);
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_beq(const Instruction &ins, const std::vector<Byte> &data) {
    if(_flag.getZero()) {
        _pc = _get_data(ins, data);
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_bne(const Instruction &ins, const std::vector<Byte> &data) {
    if(!_flag.getZero()) {
        _pc = _get_data(ins, data);
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_sta(const Instruction &ins, const std::vector<Byte> &data) {
    _mem.write(_get_data(ins, data), _reg_a);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_bit(const Instruction &ins, const std::vector<Byte> &data) {
    int d = _get_data(ins, data);
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setOverflow((_reg_a >> 6 & 1) == 1);
    if(_reg_a == (Byte)d) {
        _flag.setZero(true);
    }
    _pc += ins.length();
    _now_cycle += ins.getCycle();

}

void Cpu::_bvs(const Instruction &ins, const std::vector<Byte> &data) {
    if(_flag.getOverflow()) {
        _pc = _get_data(ins, data);
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_bvc(const Instruction &ins, const std::vector<Byte> &data) {
    if(!_flag.getOverflow()) {
        _pc = _get_data(ins, data);
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_bpl(const Instruction &ins, const std::vector<Byte> &data) {
    if(!_flag.getNegative()) {
        _pc = _get_data(ins, data);
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_rts(const Instruction &ins, const std::vector<Byte> &data) {
    _pc = combine(_stack_pop(), _stack_pop());
    _now_cycle += ins.getCycle();
}

void Cpu::_sei(const Instruction &ins, const std::vector<Byte> &data) {
    _flag.setIntDisable(true);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_sed(const Instruction &ins, const std::vector<Byte> &data) {
    _flag.setDecimal(true);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_php(const Instruction &ins, const std::vector<Byte> &data) {
    _stack_push(_flag.toByte());
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_pla(const Instruction &ins, const std::vector<Byte> &data) {
    _reg_a = _stack_pop();
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_and(const Instruction &ins, const std::vector<Byte> &data) {
    _reg_a = _reg_a & _get_data(ins, data);
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_cmp(const Instruction &ins, const std::vector<Byte> &data) {
    int diff = (int)_reg_a - _get_data(ins, data);
    _flag.setNegative(diff < 0);
    _flag.setZero(diff == 0);
    _flag.setCarry(diff <= 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}
