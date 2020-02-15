#include "cpu.h"
#include "instruction.h"
#include <cassert>

Cpu::Cpu(PtrToBus bus) : m_a(0), m_x(0), m_y(0), m_sp(0xfd), m_cycle(7), m_bus(bus)
{
    m_pc = 0xC000;
    //m_pc = m_bus->read(0xfffc) + m_bus->read(0xfffd) * 256;
    m_flag.setIntDisable(true);
}

bool Cpu::_process_ir()
{
    if (m_flag.getIntDisable() && !m_nmi && !m_rst)
    {
        //disable int
        return false;
    }
    if (m_rst)
    {
        m_rst = false;
        //do rst
        m_sp = 0xFF;
        _stack_push(high(m_pc));
        _stack_push(low(m_pc));
        _stack_push(m_flag.toByte());
        int addr = combine(m_bus->read(0xfffc), m_bus->read(0xfffd));
        m_pc = addr;
        printf("RST ADDR: %x\n", m_pc);
        m_cycle = 7;
        return true;
    }
    else if (m_nmi)
    {
        m_nmi = false;
        //do nmi
        //TODO check push flag
        // printf("NMI: %4X\n", m_pc);
        _stack_push(high(m_pc));
        _stack_push(low(m_pc));
        _stack_push(m_flag.toByte());
        int addr = combine(m_bus->read(0xfffa), m_bus->read(0xfffb));
        m_pc = addr;
        m_cycle += 7;
        return true;
    }
    else if (m_irq)
    {
        //do irq brk
        if (m_irq) {
            m_irq = false;
        }
        _stack_push(high(m_pc));
        _stack_push(low(m_pc));
        _stack_push(m_flag.toByte());
        int addr = combine(m_bus->read(0xfffe), m_bus->read(0xffff));
        m_pc = addr;
        m_cycle += 7;
        return true;
    }
    return false;
}

std::string Cpu::decode(uint16_t addr) const
{
    constexpr char fmt[][32] = {
            "$%04X: %s #$%02x", //imm 0
            "$%04X: %s $%02x", //zp 1
            "$%04X: %s $%02x, X", //zpx 2
            "$%04X: %s $%02x, Y", //zpy 3
            "$%04X: %s ($%02x, X)", //izx 4
            "$%04X: %s ($%02x), Y", //izy 5
            "$%04X: %s $%04x", //abs 6
            "$%04X: %s $%04x, X", //abx 7
            "$%04X: %s $%04x, Y", //aby 8
            "$%04X: %s ($%04x)", //ind 9
            "$%04X: %s $%02x", //rel 10
            "$%04X: %s%*", //imp 11
    };
    
    Byte op = m_bus->read(addr);
    auto ins = Instruction::INS[op];

    int index = 0;
    switch (ins.getType()) {
        case InsType::IMM: index = 0; break;
        case InsType::ZP: index = 1; break;
        case InsType::ZPX: index = 2; break;
        case InsType::ZPY: index = 3; break;
        case InsType::IZX: index = 4; break;
        case InsType::IZY: index = 5; break;
        case InsType::ABS: index = 6; break;
        case InsType::ABX: index = 7; break;
        case InsType::ABY: index = 8; break;
        case InsType::IND: index = 9; break;
        case InsType::REL: index = 10; break;
        case InsType::IMP: index = 11; break;
    }

    char buf[32] = {0};
    auto extra_data = m_bus->read(addr, ins.length());
    int ext = 0;
    if(extra_data.size() == 2) {
        ext = extra_data[1];
    } else if (extra_data.size() == 3) {
        ext = combine(extra_data[1], extra_data[2]);
    }
    sprintf(buf, fmt[index], addr, ins.getName().c_str(), ext);
    return std::string(buf);
}

bool Cpu::_cross_boundary(const Instruction &ins, const std::vector<Byte> &data) const
{
    if (ins.getType() == InsType::IZY)
    {
        int l = m_bus->read(data[1]);
        int tmp = m_y + l;
        return (tmp > 0xff);
    }
    else if (ins.getType() == InsType::ABY)
    {
        int l = data[1];
        int tmp = m_y + l;
        return (tmp > 0xff);
    }
    else if (ins.getType() == InsType::ABX)
    {
        int l = data[1];
        int tmp = m_x + l;
        return (tmp > 0xff);
    }
    else if (ins.getType() == InsType::REL)
    {
        int nxt_op_addr = m_pc + ins.length();
        int jmp_addr = m_pc + (char)data[1] + ins.length();
        return ((nxt_op_addr & 0xff00) != (jmp_addr & 0xff00));
    }
    else
    {
        return false;
    }
}

std::pair<uint16_t, uint8_t> Cpu::_get_data(const Instruction &ins, const std::vector<Byte> &data, bool need_data) const
{
    uint16_t a = 0;
    uint8_t v = 0;
    int index;
    int l, h;
    switch (ins.getType())
    {
    case InsType::IMM:
        v = data[1];
        break;
    case InsType::ZP:
        a = data[1];
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::ZPX:
        a = data[1] + m_x;
        a %= 256;
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::ZPY:
        a = data[1] + m_y;
        a %= 256;
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::IZX:
        index = (data[1] + m_x);
        index = (Byte)index;
        a = combine(m_bus->read(index), m_bus->read((index + 1) & 0xFF)); //This is a bug in 6052.
        a %= 65536;
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::IZY:
        l = m_bus->read(data[1]);
        h = m_bus->read((data[1] + 1) % 256); //This is a bug in 6052.
        a = combine(l, h) + m_y;
        a %= 65536;
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::ABS:
        a = combine(data[1], data[2]);
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::ABX:
        a = combine(data[1], data[2]) + m_x;
        a %= 65536;
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::ABY:
        a = combine(data[1], data[2]) + m_y;
        a %= 65536;
        if (need_data) v = m_bus->read(a);
        break;
    case InsType::IND:
        l = data[1];
        h = data[2];
        a = combine(l, h);
        if (l != 0xFF)
            a = combine(m_bus->read(a), m_bus->read(a + 1));
        else
            a = combine(m_bus->read(a), m_bus->read(h << 8)); //This is a bug in 6502.
        break;
    case InsType::REL:
        //WTF, see data[1] as signed number
        a = m_pc + (char)data[1] + ins.length();
        break;
    case InsType::IMP:
        break;
    }
    return {a, v};
}

int Cpu::run()
{
    int prev_cycle = m_cycle;
    if (_process_ir())
    {
        return m_cycle - prev_cycle;
    }
    Byte op = m_bus->read(m_pc);
    auto ins = Instruction::INS[op];
    // printf("$%04X: (%02X)%s\n", m_pc, op, ins.getName().c_str());
    int len = ins.length();
    auto data = m_bus->read(m_pc, len);
    _run_op(op, ins, data);
    return m_cycle - prev_cycle;
}

std::string Cpu::log() const
{
    assert(false);
    Byte op = m_bus->read(m_pc);
    auto ins = Instruction::INS[op];
    int len = ins.length();
    auto ins_data = m_bus->read(m_pc, len);
    auto msg = decode(op);
    char data[32] = {0};
    if (len == 1)
    {
        sprintf(data, "%02x", ins_data[0]);
    }
    else if (len == 2)
    {
        sprintf(data, "%02x %02x", ins_data[0], ins_data[1]);
    }
    else if (len == 3)
    {
        sprintf(data, "%02x %02x %02x", ins_data[0], ins_data[1], ins_data[2]);
    }
    else
    {
        assert(!"LEN ERROR");
    }
    printf("$%04x: [%s](%s) {A: $%02x X: $%02x Y: $%02x P: $%02x SP: $%02x CYC: %d}\n",
           m_pc, data, msg.c_str(), m_a, m_x, m_y, m_flag.toInt(), m_sp, m_cycle);
    char res[64] = {0};
    sprintf(res, "%04x%02x%02x%02x%02x%02x%d", m_pc, m_a, m_x, m_y, m_flag.toByte(), m_sp, m_cycle);
    return res;
}

void Cpu::_stack_push(Byte byte)
{
    m_bus->write(256 + m_sp--, byte);
}

Byte Cpu::_stack_pop()
{
    return m_bus->read(++m_sp + 256);
}

void Cpu::_jmp(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "JMP");
    auto d = _get_data(ins, data, false);
    m_pc = d.first;
    m_cycle += ins.getCycle();
}

void Cpu::_ldx(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "LDX");
    int need_more_cycle = 0;
    if (_cross_boundary(ins, data))
        need_more_cycle = 1;
    auto d = _get_data(ins, data, true);
    m_x = d.second;
    m_flag.setZero(m_x == 0);
    m_flag.setNegative((m_x >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle() + need_more_cycle;
}

void Cpu::_stx(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "STX");
    auto d = _get_data(ins, data, false);
    m_bus->write(d.first, m_x);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_jsr(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "JSR");
    unsigned nxt = m_pc + ins.length() - 1;
    _stack_push(high(nxt));
    _stack_push(low(nxt));
    m_pc = _get_data(ins, data, false).first;
    m_cycle += ins.getCycle();
}

void Cpu::_nop(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "NOP");
    int need_more_cycle = 0;
    if (_cross_boundary(ins, data))
        need_more_cycle = 1;
    m_pc += ins.length();
    m_cycle += ins.getCycle() + need_more_cycle;
}

void Cpu::_sec(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "SEC");
    m_flag.setCarry(true);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_bcs(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BCS");
    if (m_flag.getCarry())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_clc(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "CLC");
    m_flag.setCarry(false);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_bcc(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BCC");
    if (!m_flag.getCarry())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_lda(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "LDA");
    int more_cycle = 0;
    if (_cross_boundary(ins, data))
        more_cycle += 1;
    m_a = _get_data(ins, data, true).second;
    m_flag.setZero(m_a == 0);
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle() + more_cycle;
}

void Cpu::_beq(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BEQ");
    int need_more_cycle = 0;
    if (_cross_boundary(ins, data))
        need_more_cycle = 1;
    if (m_flag.getZero())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1 + need_more_cycle;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_bne(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BNE");
    if (!m_flag.getZero())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_sta(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "STA");
    int addr = _get_data(ins, data, false).first;
    m_bus->write(addr, m_a);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_bit(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BIT");
    auto d = _get_data(ins, data, true);
    int a = d.first, v = d.second;
    m_flag.setNegative((v >> 7 & 1) == 1);
    m_flag.setOverflow((v >> 6 & 1) == 1);
    m_flag.setZero((v & m_a) == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_bvs(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BVS");
    if (m_flag.getOverflow())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_bvc(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BVC");
    if (!m_flag.getOverflow())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_bpl(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BPL");
    if (!m_flag.getNegative())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_rts(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "RTS");
    Byte low = _stack_pop(), high = _stack_pop();
    m_pc = combine(low, high) + 1;
    m_cycle += ins.getCycle();
}

void Cpu::_sei(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "SEI");
    m_flag.setIntDisable(true);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_sed(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "SED");
    m_flag.setDecimal(true);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_php(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "PHP");
    Byte flag = m_flag.toByte();
    _stack_push(flag | (1 << 4));
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_pla(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "PLA");
    m_a = _stack_pop();
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_and(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "AND");
    m_a = m_a & _get_data(ins, data, true).second;
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_cmp(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "CMP");
    Byte v = _get_data(ins, data, true).second;
    m_flag.setNegative(((m_a - v) >> 7 & 1) == 1);
    m_flag.setZero(m_a == v);
    m_flag.setCarry(m_a >= v);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_cld(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "CLD");
    m_flag.setDecimal(false);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_pha(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "PHA");
    _stack_push(m_a);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_plp(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "PLP");
    Byte x = _stack_pop() | (1 << 5);
    bool b = m_flag.getBreak();
    m_flag.setNum(x);
    m_flag.setBreak(b);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_bmi(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "BMI");
    if (m_flag.getNegative())
    {
        m_pc = _get_data(ins, data, false).first;
        m_cycle += ins.getCycle() + 1;
    }
    else
    {
        m_pc += ins.length();
        m_cycle += ins.getCycle();
    }
}

void Cpu::_ora(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "ORA");
    auto d = _get_data(ins, data, true);
    int v = d.second;
    m_a |= v;
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_clv(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "CLV");
    m_flag.setOverflow(false);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_eor(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "EOR");
    m_a ^= _get_data(ins, data, true).second;
    m_flag.setZero(m_a == 0);
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_adc(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "ADC");
    Byte v = _get_data(ins, data, true).second;
    int ul = v, ur = m_a;
    int l = (char)v, r = char(m_a);
    int res = l + r + (int)m_flag.getCarry();
    m_a = res;
    m_flag.setCarry(ul + ur + (int)m_flag.getCarry() > 255);
    m_flag.setOverflow(res < -128 || res > 127);
    m_flag.setNegative((res >> 7 & 1) == 1);
    m_flag.setZero(res == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_ldy(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "LDY");
    int need_more_cycle = 0;
    if (_cross_boundary(ins, data))
        need_more_cycle = 1;
    Byte v = _get_data(ins, data, true).second;
    m_y = v;
    m_flag.setZero(m_y == 0);
    m_flag.setNegative((m_y >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle() + need_more_cycle;
}

void Cpu::_cpy(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "CPY");
    Byte v = _get_data(ins, data, true).second;
    m_flag.setNegative(((m_y - v) >> 7 & 1) == 1);
    m_flag.setZero(m_y == v);
    m_flag.setCarry(m_y >= v);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_cpx(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "CPX");
    Byte v = _get_data(ins, data, true).second;
    m_flag.setNegative(((m_x - v) >> 7 & 1) == 1);
    m_flag.setZero(m_x == v);
    m_flag.setCarry(m_x >= v);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_sbc(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "SBC");
    Byte per_reg = m_a;
    Byte v = _get_data(ins, data, true).second;
    int l = (char)m_a, r = -char(v);
    int res = l + r - (int)!m_flag.getCarry();
    m_a = res;
    bool carry = m_flag.getCarry();
    m_flag.setCarry(per_reg > v || (per_reg == v && carry));
    m_flag.setOverflow(res < -128 || res > 127);
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_iny(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "INY");
    m_y += 1;
    m_flag.setNegative((m_y >> 7 & 1) == 1);
    m_flag.setZero(m_y == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_inx(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "INX");
    m_x += 1;
    m_flag.setNegative((m_x >> 7 & 1) == 1);
    m_flag.setZero(m_x == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_dey(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "DEY");
    m_y -= 1;
    m_flag.setNegative((m_y >> 7 & 1) == 1);
    m_flag.setZero(m_y == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_dex(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "DEX");
    m_x -= 1;
    m_flag.setNegative((m_x >> 7 & 1) == 1);
    m_flag.setZero(m_x == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_tay(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "TAY");
    m_y = m_a;
    m_flag.setNegative((m_y >> 7 & 1) == 1);
    m_flag.setZero(m_y == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_tax(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "TAX");
    m_x = m_a;
    m_flag.setNegative((m_x >> 7 & 1) == 1);
    m_flag.setZero(m_x == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_tya(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "TYA");
    m_a = m_y;
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_txa(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "TXA");
    m_a = m_x;
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_tsx(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "TSX");
    m_x = m_sp;
    m_flag.setNegative((m_x >> 7 & 1) == 1);
    m_flag.setZero(m_x == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_txs(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "TXS");
    m_sp = m_x;
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_rti(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "RTI");
    Byte s0 = _stack_pop();
    Byte s1 = _stack_pop();
    Byte s2 = _stack_pop();
    m_pc = combine(s1, s2);
    bool _f4 = m_flag.getBreak();
    s0 |= (1 << 5);
    m_flag.setNum(s0);
    m_flag.setBreak(_f4); //ignore 4 and 5, but bit 5 is always 1.
    m_cycle += ins.getCycle();
    // printf("RTI: $%04X\n", m_pc);
}

void Cpu::_lsr(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "LSR");
    if (ins.getType() == InsType::IMP)
    {
        bool carry = (m_a & 1);
        m_a = m_a >> 1;
        m_flag.setCarry(carry);
        m_flag.setNegative(false);
        m_flag.setZero(m_a == 0);
    }
    else
    {
        auto d = _get_data(ins, data, true);
        int a = d.first;
        Byte v = d.second;
        bool carry = (v & 1);
        m_bus->write(a, v >> 1);
        m_flag.setCarry(carry);
        m_flag.setNegative(false);
        m_flag.setZero(m_bus->read(a) == 0);
    }
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_asl(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "ASL");
    if (ins.getType() == InsType::IMP)
    {
        bool carry = (m_a >> 7 & 1);
        m_a = m_a << 1;
        m_flag.setCarry(carry);
        m_flag.setNegative((m_a >> 7 & 1) == 1);
        m_flag.setZero(m_a == 0);
    }
    else
    {
        auto d = _get_data(ins, data, true);
        int a = d.first;
        Byte v = d.second;
        bool carry = (v >> 7 & 1);
        m_bus->write(a, v << 1);
        m_flag.setCarry(carry);
        m_flag.setNegative((m_bus->read(a) >> 7 & 1) == 1);
        m_flag.setZero(m_bus->read(a) == 0);
    }
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_ror(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "ROR");
    if (ins.getType() == InsType::IMP)
    {
        bool origin_carry = m_flag.getCarry();
        bool carry = (m_a & 1);
        m_a = m_a >> 1;
        m_a |= int(origin_carry) << 7;
        m_flag.setCarry(carry);
        m_flag.setNegative((m_a >> 7 & 1) == 1);
        m_flag.setZero(m_a == 0);
    }
    else
    {
        auto d = _get_data(ins, data, true);
        int a = d.first;
        Byte v = d.second;
        bool origin_carry = m_flag.getCarry();
        bool carry = (v & 1);
        Byte res = v;
        res = res >> 1;
        res |= int(origin_carry) << 7;
        m_bus->write(a, res);
        m_flag.setCarry(carry);
        m_flag.setNegative((res >> 7 & 1) == 1);
        m_flag.setZero(res == 0);
    }
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_rol(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "ROL");
    if (ins.getType() == InsType::IMP)
    {
        bool origin_carry = m_flag.getCarry();
        bool carry = (m_a >> 7 & 1);
        m_a = m_a << 1;
        m_a |= int(origin_carry);
        m_flag.setCarry(carry);
        m_flag.setNegative((m_a >> 7 & 1) == 1);
        m_flag.setZero(m_a == 0);
    }
    else
    {
        auto d = _get_data(ins, data, true);
        int a = d.first;
        Byte v = d.second;
        bool origin_carry = m_flag.getCarry();
        bool carry = (v >> 7 & 1);
        Byte res = v;
        res = res << 1;
        res |= int(origin_carry);
        m_bus->write(a, res);
        m_flag.setCarry(carry);
        m_flag.setNegative((res >> 7 & 1) == 1);
        m_flag.setZero(res == 0);
    }
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_sty(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "STY");
    auto d = _get_data(ins, data, false);
    int a = d.first;
    m_bus->write(a, m_y);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_inc(const Instruction &ins, const std::vector<Byte> &data)
{
    auto d = _get_data(ins, data, true);
    int a = d.first;
    Byte v = d.second;
    ++v;
    m_bus->write(a, v);
    m_flag.setZero(v == 0);
    m_flag.setNegative((v >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_dec(const Instruction &ins, const std::vector<Byte> &data)
{
    auto d = _get_data(ins, data, true);
    int a = d.first;
    Byte v = d.second;
    --v;
    m_bus->write(a, v);
    m_flag.setZero(v == 0);
    m_flag.setNegative((v >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_lax(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "LAX");
    int need_more_cycle = 0;
    if (_cross_boundary(ins, data))
        need_more_cycle = 1;
    int v = _get_data(ins, data, true).second;
    m_a = v;
    m_x = v;
    m_flag.setZero(m_a == 0);
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle() + need_more_cycle;
}

void Cpu::_sax(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "SAX");
    int a = _get_data(ins, data, false).first;
    m_bus->write(a, (m_a & m_x));
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_dcp(const Instruction &ins, const std::vector<Byte> &data)
{
    //dec addr and cmp addr
    assert(ins.getName() == "DCP");
    auto d = _get_data(ins, data, true);
    unsigned short a = d.first;
    Byte v = d.second;
    --v;
    m_bus->write(a, v);
    m_flag.setNegative(((m_a - v) >> 7 & 1) == 1);
    m_flag.setZero(m_a == v);
    m_flag.setCarry(m_a >= v);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_isc(const Instruction &ins, const std::vector<Byte> &data)
{
    //inc addr and sbc addr
    assert(ins.getName() == "ISC");
    Byte per_reg = m_a;
    auto d = _get_data(ins, data, true);
    unsigned short a = d.first;
    Byte v = d.second;
    ++v;
    m_bus->write(a, v);
    int l = (char)m_a, r = -char(v);
    int res = l + r - (int)!m_flag.getCarry();
    m_a = res;
    bool carry = m_flag.getCarry();
    m_flag.setCarry(per_reg > v || (per_reg == v && carry));
    m_flag.setOverflow(res < -128 || res > 127);
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_slo(const Instruction &ins, const std::vector<Byte> &data)
{
    assert(ins.getName() == "SLO");
    auto d = _get_data(ins, data, true);
    unsigned short a = d.first;
    Byte v = d.second;
    bool carry = (v >> 7 & 1);
    v = v << 1;
    m_bus->write(a, v);
    m_flag.setCarry(carry);
    m_flag.setNegative((v >> 7 & 1) == 1);
    m_flag.setZero(v == 0);

    m_a |= v;
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_rla(const Instruction &ins, const std::vector<Byte> &data)
{
    //ROL + AND
    assert(ins.getName() == "RLA");
    auto d = _get_data(ins, data, true);
    unsigned short a = d.first;
    Byte v = d.second;
    bool origin_carry = m_flag.getCarry();
    bool carry = (v >> 7 & 1);
    v = v << 1;
    v |= int(origin_carry);
    m_bus->write(a, v);
    m_flag.setCarry(carry);
    m_flag.setNegative((v >> 7 & 1) == 1);
    m_flag.setZero(v == 0);
    m_a = m_a & v;
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_flag.setZero(m_a == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_sre(const Instruction &ins, const std::vector<Byte> &data)
{
    auto d = _get_data(ins, data, true);
    unsigned short a = d.first;
    Byte v = d.second;
    bool carry = (v & 1);
    v = v >> 1;
    m_bus->write(a, v);
    m_flag.setCarry(carry);
    m_flag.setNegative(false);
    m_flag.setZero(v == 0);
    m_a ^= v;
    m_flag.setZero(m_a == 0);
    m_flag.setNegative((m_a >> 7 & 1) == 1);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_rra(const Instruction &ins, const std::vector<Byte> &data)
{
    //ROR + ADC
    auto d = _get_data(ins, data, true);
    unsigned short a = d.first;
    Byte v = d.second;
    bool origin_carry = m_flag.getCarry();
    bool carry = (v & 1);
    v = v >> 1;
    v |= int(origin_carry) << 7;
    m_bus->write(a, v);
    m_flag.setCarry(carry);
    m_flag.setNegative((v >> 7 & 1) == 1);
    m_flag.setZero(v == 0);
    int ul = v, ur = m_a;
    int l = (char)v, r = char(m_a);
    int res = l + r + (int)m_flag.getCarry();
    m_a = res;
    m_flag.setCarry(ul + ur + (int)m_flag.getCarry() > 255);
    m_flag.setOverflow(res < -128 || res > 127);
    m_flag.setNegative((res >> 7 & 1) == 1);
    m_flag.setZero(res == 0);
    m_pc += ins.length();
    m_cycle += ins.getCycle();
}

void Cpu::_run_op(Byte op, const Instruction& ins, const std::vector<Byte>& data) {
switch (op)
    {
    case 0x4c:
    case 0x6c:
        _jmp(ins, data);
        break;
    case 0xa2:
    case 0xae:
    case 0xa6:
    case 0xb6:
    case 0xbe:
        _ldx(ins, data);
        break;
    case 0x86:
    case 0x8e:
    case 0x96:
        _stx(ins, data);
        break;
    case 0x20:
        _jsr(ins, data);
        break;
    case 0xea:
    case 0x04:
    case 0x44:
    case 0x64:
    case 0x0c:
    case 0x14:
    case 0x34:
    case 0x54:
    case 0x74:
    case 0xd4:
    case 0xf4:
    case 0x1a:
    case 0x3a:
    case 0x5a:
    case 0x7a:
    case 0xda:
    case 0xfa:
    case 0x80:
    case 0x1c:
    case 0x3c:
    case 0x5c:
    case 0x7c:
    case 0xdc:
    case 0xfc:
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
    case 0xb5:
    case 0xbd:
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
    case 0x99:
    case 0x95:
    case 0x9d:
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
    case 0x39:
    case 0x35:
    case 0x3d:
        _and(ins, data);
        break;
    case 0xc9:
    case 0xc1:
    case 0xc5:
    case 0xcd:
    case 0xd1:
    case 0xd9:
    case 0xd5:
    case 0xdd:
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
    case 0x19:
    case 0x15:
    case 0x1d:
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
    case 0x59:
    case 0x55:
    case 0x5d:
        _eor(ins, data);
        break;
    case 0x69:
    case 0x61:
    case 0x65:
    case 0x6d:
    case 0x71:
    case 0x79:
    case 0x75:
    case 0x7d:
        _adc(ins, data);
        break;
    case 0xa0:
    case 0xa4:
    case 0xac:
    case 0xb4:
    case 0xbc:
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
    case 0xf9:
    case 0xf5:
    case 0xfd:
    case 0xeb:
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
    case 0x56:
    case 0x5e:
        _lsr(ins, data);
        break;
    case 0x0a:
    case 0x06:
    case 0x0e:
    case 0x16:
    case 0x1e:
        _asl(ins, data);
        break;
    case 0x6a:
    case 0x66:
    case 0x6e:
    case 0x76:
    case 0x7e:
        _ror(ins, data);
        break;
    case 0x2a:
    case 0x26:
    case 0x2e:
    case 0x36:
    case 0x3e:
        _rol(ins, data);
        break;
    case 0x84:
    case 0x8c:
    case 0x94:
        _sty(ins, data);
        break;
    case 0xe6:
    case 0xee:
    case 0xf6:
    case 0xfe:
        _inc(ins, data);
        break;
    case 0xc6:
    case 0xce:
    case 0xd6:
    case 0xde:
        _dec(ins, data);
        break;
    case 0xa3:
    case 0xa7:
    case 0xaf:
    case 0xb3:
    case 0xb7:
    case 0xbf:
        _lax(ins, data);
        break;
    case 0x83:
    case 0x87:
    case 0x8f:
    case 0x97:
        _sax(ins, data);
        break;
    case 0xc3:
    case 0xc7:
    case 0xcf:
    case 0xd3:
    case 0xd7:
    case 0xdb:
    case 0xdf:
        _dcp(ins, data);
        break;
    case 0xe3:
    case 0xe7:
    case 0xef:
    case 0xf3:
    case 0xf7:
    case 0xfb:
    case 0xff:
        _isc(ins, data);
        break;
    case 0x03:
    case 0x07:
    case 0x0f:
    case 0x13:
    case 0x17:
    case 0x1b:
    case 0x1f:
        _slo(ins, data);
        break;
    case 0x23:
    case 0x27:
    case 0x2f:
    case 0x33:
    case 0x37:
    case 0x3b:
    case 0x3f:
        _rla(ins, data);
        break;
    case 0x43:
    case 0x47:
    case 0x4f:
    case 0x53:
    case 0x57:
    case 0x5b:
    case 0x5f:
        _sre(ins, data);
        break;
    case 0x63:
    case 0x67:
    case 0x6f:
    case 0x73:
    case 0x77:
    case 0x7b:
    case 0x7f:
        _rra(ins, data);
        break;
    default:
        printf("UNKNOWN OP $%02x!", op);
        exit(-1);
    }
}

std::map<uint16_t, std::string> Cpu::asmCode(int startAddr, int endAddr) const {
    int now = startAddr;
    std::map<uint16_t, std::string> res;
    while (now < endAddr) {
        int op = m_bus->read(now);
        auto ins = Instruction::INS[op];
        res[now] = decode(now);
        now += ins.length();
    }
    return res;
}