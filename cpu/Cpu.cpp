//
// Created by DELL on 2019/8/29.
//

#include "Cpu.h"
#include "Instruction.h"
#include <cassert>


std::string Cpu::decode(Byte op) const {
//    constexpr char fmt[][20] = {
//            "%s #$%02x", //imm 0
//            "%s $%02x", //zp 1
//            "%s $%02x, X", //zpx 2
//            "%s $%02x, Y", //zpy 3
//            "%s ($%02x, X)", //izx 4
//            "%s ($%02x), Y", //izy 5
//            "%s $%04x", //abs 6
//            "%s $%04x, X", //abx 7
//            "%s $%04x, Y", //aby 8
//            "%s ($%04x)", //ind 9
//            "%s $%04x", //rel 10
//            "%s", //imp 11
//    };
    auto ins = Instruction::INS[op];
    char buf[32] = {0};
    auto data = _mem.read(_pc, ins.length());
    auto d = _get_data(ins, data);
    sprintf(buf, "%s a: %04x v: %02x", ins.getName().c_str(), d.first, d.second);
    return buf;
}

bool Cpu::_cross_boundary(const Instruction &ins, const std::vector<Byte> &data) const {
    //TODO: NEED BUG FIX
    if(ins.getType() == InsType::IZY) {
        int low = _mem.read(data[1]);
        //int high = _mem.read((data[1] + 1) % 256); //This is a bug in 6052.
        return (low + _reg_y > 0xff);
    } else if(ins.getType() == InsType::ABY) {
        int low = _mem.read(data[1]);
        return (low + _reg_y > 0xff);
    } else if(ins.getType() == InsType::ABX) {
        int low = _mem.read(data[1]);
        return (low + _reg_x > 0xff);
    } else {
        return false;
    }
}

std::pair<int, int> Cpu::_get_data(const Instruction &ins, const std::vector<Byte> &data) const {
    int a = 0x3f3f, v = 0x3f3f, index;
    int l, r;
    switch (ins.getType()) {
        case InsType::IMM:
            v = data[1];
            break;
        case InsType::ZP:
            a = data[1];
            v = _mem.read(a);
            break;
        case InsType::ZPX:
            assert(!"ZPX");
            break;
        case InsType::ZPY:
            assert(!"ZPY");
            break;
        case InsType::IZX:
            index = (data[1] + _reg_x);
            index = (Byte)index;
            a = combine(_mem.read(index), _mem.read((index + 1) & 0xFF)); //This is a bug in 6052.
            a %= 65536;
            v = _mem.read(a);
            break;
        case InsType::IZY:
            l = _mem.read(data[1]);
            r = _mem.read((data[1] + 1) % 256); //This is a bug in 6052.
            a = combine(l, r) + _reg_y;
            a %= 65536;
            v = _mem.read(a);
            break;
        case InsType::ABS:
            a = combine(data[1], data[2]);
            v = _mem.read(a);
            break;
        case InsType::ABX:
            a = combine(data[1], data[2]) + _reg_x;
            a %= 65536;
            v = _mem.read(a);
            break;
        case InsType::ABY:
            a = combine(data[1], data[2]) + _reg_y;
            a %= 65536;
            v = _mem.read(a);
            break;
        case InsType::IND:
            a = combine(data[1], data[2]);
            l = low(a); r = high(a);
            a = combine(_mem.read(a), _mem.read(r * 256 + (l + 1) % 256)); //This is a bug in 6502.
            break;
        case InsType::REL:
            a = _pc + (int) data[1] + ins.length();
            break;
        case InsType::IMP:
            break;
    }
    return {a, v};
}

Cpu::Cpu(Memory &memory) : _reg_a(0), _reg_x(0), _reg_y(0), _sp(0xfd), _now_cycle(7), _mem(memory) {
    _pc = 0xc000; //test
    //_pc = _mem.read(0xfffc) + _mem.read(0xfffd) * 256;
    _flag.setIntDisable(true);
}

int Cpu::run() {
    Byte op = _mem.read(_pc);
    auto ins = Instruction::INS[op];
    int len = ins.length();
    auto data = _mem.read(_pc, len);
    switch (op) {
        case 0x4c:
        case 0x6c:
            _jmp(ins, data);
            break;
        case 0xa2:
        case 0xae:
        case 0xa6:
            _ldx(ins, data);
            break;
        case 0x86:
        case 0x8e:
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
        case 0xad:
        case 0xa5:
        case 0xa1:
        case 0xb1:
        case 0xb9:
            _lda(ins, data);
            break;
        case 0xf0:
            _beq(ins, data);
            break;
        case 0xd0:
            _bne(ins, data);
            break;
        case 0x85:
        case 0x8d:
        case 0x81:
        case 0x91:
            _sta(ins, data);
            break;
        case 0x24:
        case 0x2c:
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
        case 0x21:
        case 0x25:
        case 0x2d:
        case 0x31:
            _and(ins, data);
            break;
        case 0xc9:
        case 0xc1:
        case 0xc5:
        case 0xcd:
        case 0xd1:
            _cmp(ins, data);
            break;
        case 0xd8:
            _cld(ins, data);
            break;
        case 0x48:
            _pha(ins, data);
            break;
        case 0x28:
            _plp(ins, data);
            break;
        case 0x30:
            _bmi(ins, data);
            break;
        case 0x09:
        case 0x01:
        case 0x05:
        case 0x0d:
        case 0x11:
            _ora(ins, data);
            break;
        case 0xb8:
            _clv(ins, data);
            break;
        case 0x49:
        case 0x41:
        case 0x45:
        case 0x4d:
        case 0x51:
            _eor(ins, data);
            break;
        case 0x69:
        case 0x61:
        case 0x65:
        case 0x6d:
        case 0x71:
            _adc(ins, data);
            break;
        case 0xa0:
        case 0xa4:
        case 0xac:
            _ldy(ins, data);
            break;
        case 0xc0:
        case 0xc4:
        case 0xcc:
            _cpy(ins, data);
            break;
        case 0xe0:
        case 0xe4:
        case 0xec:
            _cpx(ins, data);
            break;
        case 0xe9:
        case 0xe1:
        case 0xe5:
        case 0xed:
        case 0xf1:
            _sbc(ins, data);
            break;
        case 0xc8:
            _iny(ins, data);
            break;
        case 0xe8:
            _inx(ins, data);
            break;
        case 0x88:
            _dey(ins, data);
            break;
        case 0xca:
            _dex(ins, data);
            break;
        case 0xa8:
            _tay(ins, data);
            break;
        case 0xaa:
            _tax(ins, data);
            break;
        case 0x98:
            _tya(ins, data);
            break;
        case 0x8a:
            _txa(ins, data);
            break;
        case 0xba:
            _tsx(ins, data);
            break;
        case 0x9a:
            _txs(ins, data);
            break;
        case 0x40:
            _rti(ins, data);
            break;
        case 0x4a:
        case 0x46:
        case 0x4e:
            _lsr(ins, data);
            break;
        case 0x0a:
        case 0x06:
        case 0x0e:
            _asl(ins, data);
            break;
        case 0x6a:
        case 0x66:
        case 0x6e:
            _ror(ins, data);
            break;
        case 0x2a:
        case 0x26:
        case 0x2e:
            _rol(ins, data);
            break;
        case 0x84:
        case 0x8c:
            _sty(ins, data);
            break;
        case 0xe6:
        case 0xee:
            _inc(ins, data);
            break;
        case 0xc6:
        case 0xce:
            _dec(ins, data);
            break;
        default:
            printf("UNKNOWN OP $%02x!", op);
            exit(-1);
    }
    return 0;
}

std::string Cpu::log() const {
    Byte op = _mem.read(_pc);
    auto ins = Instruction::INS[op];
    int len = ins.length();
    auto ins_data = _mem.read(_pc, len);
    auto msg = decode(op);
    char data[32] = {0};
    if (len == 1) {
        sprintf(data, "%02x", ins_data[0]);
    } else if (len == 2) {
        sprintf(data, "%02x %02x", ins_data[0], ins_data[1]);
    } else if (len == 3) {
        sprintf(data, "%02x %02x %02x", ins_data[0], ins_data[1], ins_data[2]);
    } else {
        assert(!"LEN ERROR");
    }
    printf("$%04x: [%s](%s) {A: $%02x X: $%02x Y: $%02x P: $%02x SP: $%02x CYC: %d}\n",
           _pc, data, msg.c_str(), _reg_a, _reg_x, _reg_y, _flag.toInt(), _sp, _now_cycle);
    char res[64] = {0};
    sprintf(res, "%04x%02x%02x%02x%02x%02x%d", _pc, _reg_a, _reg_x, _reg_y, _flag.toByte(), _sp, _now_cycle);
    return res;
}

void Cpu::_stack_push(Byte byte) {
    _mem.write(256 + _sp--, byte);
}

Byte Cpu::_stack_pop() {
    return _mem.read(++_sp + 256);
}


void Cpu::_jmp(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "JMP");
    auto d = _get_data(ins, data);
    _pc = d.first;
    _now_cycle += ins.getCycle();
}


void Cpu::_ldx(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "LDX");
    auto d = _get_data(ins, data);
    _reg_x = d.second;
    _flag.setZero(_reg_x == 0);
    _flag.setNegative((_reg_x >> 7 & 1) == 1);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_stx(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "STX");
    auto d = _get_data(ins, data);
    _mem.write(d.first, _reg_x);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_jsr(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "JSR");
    unsigned nxt = _pc + ins.length() - 1;
    _stack_push(high(nxt));
    _stack_push(low(nxt));
    _pc = _get_data(ins, data).first;
    _now_cycle += ins.getCycle();
}

void Cpu::_nop(const Instruction &ins, const std::vector<Byte> &data) {
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

void Cpu::_bcs(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BCS");
    if (_flag.getCarry()) {
        _pc = _get_data(ins, data).first;
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
    if (!_flag.getCarry()) {
        _pc = _get_data(ins, data).first;
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_lda(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "LDA");
    int more_cycle = 0;
    if(_cross_boundary(ins, data)) more_cycle += 1;
    _reg_a = _get_data(ins, data).second;
    _flag.setZero(_reg_a == 0);
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _pc += ins.length();
    _now_cycle += ins.getCycle() + more_cycle;
}

void Cpu::_beq(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BEQ");
    if (_flag.getZero()) {
        _pc = _get_data(ins, data).first;
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_bne(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BNE");
    if (!_flag.getZero()) {
        _pc = _get_data(ins, data).first;
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_sta(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "STA");
    int addr = _get_data(ins, data).first;
    _mem.write(addr, _reg_a);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_bit(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BIT");
    auto d = _get_data(ins, data);
    int a = d.first, v = d.second;
    _flag.setNegative((v >> 7 & 1) == 1);
    _flag.setOverflow((v >> 6 & 1) == 1);
    _flag.setZero((v & _reg_a) == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();

}

void Cpu::_bvs(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BVS");
    if (_flag.getOverflow()) {
        _pc = _get_data(ins, data).first;
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_bvc(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BVC");
    if (!_flag.getOverflow()) {
        _pc = _get_data(ins, data).first;
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_bpl(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BPL");
    if (!_flag.getNegative()) {
        _pc = _get_data(ins, data).first;
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_rts(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "RTS");
    Byte low = _stack_pop(), high = _stack_pop();
    _pc = combine(low, high) + 1;
    _now_cycle += ins.getCycle();
}

void Cpu::_sei(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "SEI");
    _flag.setIntDisable(true);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_sed(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "SED");
    _flag.setDecimal(true);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_php(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "PHP");
    Byte flag = _flag.toByte();
    _stack_push(flag | (1 << 4));
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_pla(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "PLA");
    _reg_a = _stack_pop();
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_and(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "AND");
    _reg_a = _reg_a & _get_data(ins, data).second;
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_cmp(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "CMP");
    Byte v = _get_data(ins, data).second;
    _flag.setNegative(((_reg_a - v) >> 7 & 1) == 1);
    _flag.setZero(_reg_a == v);
    _flag.setCarry(_reg_a >= v);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_cld(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "CLD");
    _flag.setDecimal(false);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_pha(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "PHA");
    _stack_push(_reg_a);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_plp(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "PLP");
    Byte x = _stack_pop() | (1 << 5);
    bool b = _flag.getBreak();
    _flag.setNum(x);
    _flag.setBreak(b);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_bmi(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "BMI");
    if (_flag.getNegative()) {
        _pc = _get_data(ins, data).first;
        _now_cycle += ins.getCycle() + 1;
    } else {
        _pc += ins.length();
        _now_cycle += ins.getCycle();
    }
}

void Cpu::_ora(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "ORA");
    auto d = _get_data(ins, data);
    int v = d.second;
    _reg_a |= v;
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_clv(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "CLV");
    _flag.setOverflow(false);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_eor(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "EOR");
    _reg_a ^= _get_data(ins, data).second;
    _flag.setZero(_reg_a == 0);
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_adc(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "ADC");
    Byte v = _get_data(ins, data).second;
    int ul = v, ur = _reg_a;
    int l = (char) v, r = char(_reg_a);
    int res = l + r + (int) _flag.getCarry();
    _reg_a = res;
    _flag.setCarry(ul + ur + (int) _flag.getCarry() > 255);
    _flag.setOverflow(res < -128 || res > 127);
    _flag.setNegative((res >> 7 & 1) == 1);
    _flag.setZero(res == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_ldy(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "LDY");
    Byte v = _get_data(ins, data).second;
    _reg_y = v;
    _flag.setZero(_reg_y == 0);
    _flag.setNegative((_reg_y >> 7 & 1) == 1);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_cpy(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "CPY");
    Byte v = _get_data(ins, data).second;
    _flag.setNegative(((_reg_y - v) >> 7 & 1) == 1);
    _flag.setZero(_reg_y == v);
    _flag.setCarry(_reg_y >= v);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_cpx(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "CPX");
    Byte v = _get_data(ins, data).second;
    _flag.setNegative(((_reg_x - v) >> 7 & 1) == 1);
    _flag.setZero(_reg_x == v);
    _flag.setCarry(_reg_x >= v);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_sbc(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "SBC");
    Byte per_reg = _reg_a;
    Byte v = _get_data(ins, data).second;
    int l = (char) _reg_a, r = -char(v);
    int res = l + r - (int) !_flag.getCarry();
    _reg_a = res;
    bool carry = _flag.getCarry();
    _flag.setCarry(per_reg > v || (per_reg == v && carry));
    _flag.setOverflow(res < -128 || res > 127);
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_iny(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "INY");
    _reg_y += 1;
    _flag.setNegative((_reg_y >> 7 & 1) == 1);
    _flag.setZero(_reg_y == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_inx(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "INX");
    _reg_x += 1;
    _flag.setNegative((_reg_x >> 7 & 1) == 1);
    _flag.setZero(_reg_x == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_dey(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "DEY");
    _reg_y -= 1;
    _flag.setNegative((_reg_y >> 7 & 1) == 1);
    _flag.setZero(_reg_y == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_dex(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "DEX");
    _reg_x -= 1;
    _flag.setNegative((_reg_x >> 7 & 1) == 1);
    _flag.setZero(_reg_x == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_tay(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "TAY");
    _reg_y = _reg_a;
    _flag.setNegative((_reg_y >> 7 & 1) == 1);
    _flag.setZero(_reg_y == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_tax(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "TAX");
    _reg_x = _reg_a;
    _flag.setNegative((_reg_x >> 7 & 1) == 1);
    _flag.setZero(_reg_x == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_tya(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "TYA");
    _reg_a = _reg_y;
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_txa(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "TXA");
    _reg_a = _reg_x;
    _flag.setNegative((_reg_a >> 7 & 1) == 1);
    _flag.setZero(_reg_a == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_tsx(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "TSX");
    _reg_x = _sp;
    _flag.setNegative((_reg_x >> 7 & 1) == 1);
    _flag.setZero(_reg_x == 0);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_txs(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "TXS");
    _sp = _reg_x;
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_rti(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "RTI");
    Byte s0 = _stack_pop();
    Byte s1 = _stack_pop();
    Byte s2 = _stack_pop();
    _pc = combine(s1, s2);
    bool _f4 = _flag.getBreak();
    s0 |= (1 << 5);
    _flag.setNum(s0);
    _flag.setBreak(_f4);//ignore 4 and 5, but bit 5 is always 1.
    _now_cycle += ins.getCycle();
}

void Cpu::_lsr(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "LSR");
    if(ins.getType() == InsType::IMP) {
        bool carry = (_reg_a & 1);
        _reg_a = _reg_a >> 1;
        _flag.setCarry(carry);
        _flag.setNegative(false);
        _flag.setZero(_reg_a == 0);
    } else {
        auto d = _get_data(ins, data);
        int a = d.first;
        Byte v = d.second;
        bool carry = (v & 1);
        _mem.write(a, v >> 1);
        _flag.setCarry(carry);
        _flag.setNegative(false);
        _flag.setZero(_mem.read(a) == 0);
    }
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_asl(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "ASL");
    if(ins.getType() == InsType::IMP) {
        bool carry = (_reg_a >> 7 & 1);
        _reg_a = _reg_a << 1;
        _flag.setCarry(carry);
        _flag.setNegative((_reg_a >> 7 & 1) == 1);
        _flag.setZero(_reg_a == 0);
    } else {
        auto d = _get_data(ins, data);
        int a = d.first;
        Byte v = d.second;
        bool carry = (v >> 7 & 1);
        _mem.write(a, v << 1);
        _flag.setCarry(carry);
        _flag.setNegative((_mem.read(a) >> 7 & 1) == 1);
        _flag.setZero(_mem.read(a) == 0);
    }
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_ror(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "ROR");
    if(ins.getType() == InsType::IMP) {
        bool origin_carry = _flag.getCarry();
        bool carry = (_reg_a & 1);
        _reg_a = _reg_a >> 1;
        _reg_a |= int(origin_carry) << 7;
        _flag.setCarry(carry);
        _flag.setNegative((_reg_a >> 7 & 1) == 1);
        _flag.setZero(_reg_a == 0);
    } else {
        auto d = _get_data(ins, data);
        int a = d.first;
        Byte v = d.second;
        bool origin_carry = _flag.getCarry();
        bool carry = (v & 1);
        Byte res = v;
        res = res >> 1;
        res |= int(origin_carry) << 7;
        _mem.write(a, res);
        _flag.setCarry(carry);
        _flag.setNegative((res >> 7 & 1) == 1);
        _flag.setZero(res == 0);
    }
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_rol(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "ROL");
    if(ins.getType() == InsType::IMP) {
        bool origin_carry = _flag.getCarry();
        bool carry = (_reg_a >> 7 & 1);
        _reg_a = _reg_a << 1;
        _reg_a |= int(origin_carry);
        _flag.setCarry(carry);
        _flag.setNegative((_reg_a >> 7 & 1) == 1);
        _flag.setZero(_reg_a == 0);
    } else {
        auto d = _get_data(ins, data);
        int a = d.first;
        Byte v = d.second;
        bool origin_carry = _flag.getCarry();
        bool carry = (v >> 7 & 1);
        Byte res = v;
        res = res << 1;
        res |= int(origin_carry);
        _mem.write(a, res);
        _flag.setCarry(carry);
        _flag.setNegative((res >> 7 & 1) == 1);
        _flag.setZero(res == 0);
    }
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_sty(const Instruction &ins, const std::vector<Byte> &data) {
    assert(ins.getName() == "STY");
    auto d = _get_data(ins, data);
    int a = d.first;
    _mem.write(a, _reg_y);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_inc(const Instruction &ins, const std::vector<Byte> &data) {
    auto d = _get_data(ins, data);
    int a = d.first;
    Byte v = d.second;
    ++v;
    _mem.write(a, v);
    _flag.setZero(v == 0);
    _flag.setNegative((v >> 7 & 1) == 1);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}

void Cpu::_dec(const Instruction &ins, const std::vector<Byte> &data) {
    auto d = _get_data(ins, data);
    int a = d.first;
    Byte v = d.second;
    --v;
    _mem.write(a, v);
    _flag.setZero(v == 0);
    _flag.setNegative((v >> 7 & 1) == 1);
    _pc += ins.length();
    _now_cycle += ins.getCycle();
}