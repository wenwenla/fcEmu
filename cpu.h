#ifndef FCEMU_CPU_H
#define FCEMU_CPU_H

#include "utility.h"
#include "flag.h"
#include "bus.h"
#include "instruction.h"
#include <map>

class Cpu
{
public:
    Cpu(PtrToBus bus);
    int run();
    std::string log() const;
    std::string decode(uint16_t addr) const;

    std::map<uint16_t, std::string> asmCode(int startAddr, int endAddr) const; 

    void setIRQ() { m_irq = true; }; //$fffe
    void setNMI() { m_nmi = true; }; //$fffa
    void setRST() { m_rst = true; }; //$fffc

    uint16_t getPc() const { return m_pc; }

private:
    Byte m_a, m_x, m_y;          // 3 general purpose registers
    StatusFlag m_flag;            //status register
    unsigned short m_pc;          //pc should be 16 bits.
    Byte m_sp;
    int m_cycle;
    PtrToBus m_bus{nullptr};
    bool m_irq = false, m_nmi = false, m_rst = false;

private:
    std::pair<uint16_t, uint8_t> _get_data(const Instruction &ins, const std::vector<Byte> &data, bool need_data) const;
    void _stack_push(Byte byte);
    Byte _stack_pop();
    bool _cross_boundary(const Instruction &ins, const std::vector<Byte> &data) const;
    bool _process_ir();

    void _run_op(Byte op, const Instruction& ins, const std::vector<Byte>& data);

private:
    void _jmp(const Instruction &ins, const std::vector<Byte> &data);
    void _ldx(const Instruction &ins, const std::vector<Byte> &data);
    void _stx(const Instruction &ins, const std::vector<Byte> &data);
    void _jsr(const Instruction &ins, const std::vector<Byte> &data);
    void _nop(const Instruction &ins, const std::vector<Byte> &data);
    void _sec(const Instruction &ins, const std::vector<Byte> &data);
    void _bcs(const Instruction &ins, const std::vector<Byte> &data);
    void _clc(const Instruction &ins, const std::vector<Byte> &data);
    void _bcc(const Instruction &ins, const std::vector<Byte> &data);
    void _lda(const Instruction &ins, const std::vector<Byte> &data);
    void _beq(const Instruction &ins, const std::vector<Byte> &data);
    void _bne(const Instruction &ins, const std::vector<Byte> &data);
    void _sta(const Instruction &ins, const std::vector<Byte> &data);
    void _bit(const Instruction &ins, const std::vector<Byte> &data);
    void _bvs(const Instruction &ins, const std::vector<Byte> &data);
    void _bvc(const Instruction &ins, const std::vector<Byte> &data);
    void _bpl(const Instruction &ins, const std::vector<Byte> &data);
    void _rts(const Instruction &ins, const std::vector<Byte> &data);
    void _sei(const Instruction &ins, const std::vector<Byte> &data);
    void _sed(const Instruction &ins, const std::vector<Byte> &data);
    void _php(const Instruction &ins, const std::vector<Byte> &data);
    void _pla(const Instruction &ins, const std::vector<Byte> &data);
    void _and(const Instruction &ins, const std::vector<Byte> &data);
    void _cmp(const Instruction &ins, const std::vector<Byte> &data);
    void _cld(const Instruction &ins, const std::vector<Byte> &data);
    void _pha(const Instruction &ins, const std::vector<Byte> &data);
    void _plp(const Instruction &ins, const std::vector<Byte> &data);
    void _bmi(const Instruction &ins, const std::vector<Byte> &data);
    void _ora(const Instruction &ins, const std::vector<Byte> &data);
    void _clv(const Instruction &ins, const std::vector<Byte> &data);
    void _eor(const Instruction &ins, const std::vector<Byte> &data);
    void _adc(const Instruction &ins, const std::vector<Byte> &data);
    void _ldy(const Instruction &ins, const std::vector<Byte> &data);
    void _cpy(const Instruction &ins, const std::vector<Byte> &data);
    void _cpx(const Instruction &ins, const std::vector<Byte> &data);
    void _sbc(const Instruction &ins, const std::vector<Byte> &data);
    void _iny(const Instruction &ins, const std::vector<Byte> &data);
    void _inx(const Instruction &ins, const std::vector<Byte> &data);
    void _dey(const Instruction &ins, const std::vector<Byte> &data);
    void _dex(const Instruction &ins, const std::vector<Byte> &data);
    void _tay(const Instruction &ins, const std::vector<Byte> &data);
    void _tax(const Instruction &ins, const std::vector<Byte> &data);
    void _tya(const Instruction &ins, const std::vector<Byte> &data);
    void _txa(const Instruction &ins, const std::vector<Byte> &data);
    void _tsx(const Instruction &ins, const std::vector<Byte> &data);
    void _txs(const Instruction &ins, const std::vector<Byte> &data);
    void _rti(const Instruction &ins, const std::vector<Byte> &data);
    void _lsr(const Instruction &ins, const std::vector<Byte> &data);
    void _asl(const Instruction &ins, const std::vector<Byte> &data);
    void _ror(const Instruction &ins, const std::vector<Byte> &data);
    void _rol(const Instruction &ins, const std::vector<Byte> &data);
    void _sty(const Instruction &ins, const std::vector<Byte> &data);
    void _inc(const Instruction &ins, const std::vector<Byte> &data);
    void _dec(const Instruction &ins, const std::vector<Byte> &data);
    void _lax(const Instruction &ins, const std::vector<Byte> &data);
    void _sax(const Instruction &ins, const std::vector<Byte> &data);
    void _dcp(const Instruction &ins, const std::vector<Byte> &data);
    void _isc(const Instruction &ins, const std::vector<Byte> &data);
    void _slo(const Instruction &ins, const std::vector<Byte> &data);
    void _rla(const Instruction &ins, const std::vector<Byte> &data);
    void _sre(const Instruction &ins, const std::vector<Byte> &data);
    void _rra(const Instruction &ins, const std::vector<Byte> &data);
};

#endif //FCEMU_CPU_H
