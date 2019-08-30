//
// Created by DELL on 2019/8/29.
//

#ifndef FCEMU_CPU_H
#define FCEMU_CPU_H


#include "../utility.h"
#include "StatusFlag.h"
#include "../memory/Memory.h"
#include "Instruction.h"

class Cpu {
public:
    Cpu(Memory& memory);
    int run();

    void log() const;

    std::string decode(Byte op) const;

private:
    Byte _reg_a, _reg_x, _reg_y; // 3 general purpose registers
    StatusFlag _flag; //status register
    unsigned short _pc; //pc should be 16 bits.
    Byte _sp;
    int _now_cycle;
    Memory& _mem;

private:
    int _get_data(const Instruction &ins, const std::vector<Byte> &data) const;
    void _stack_push(Byte byte);
    Byte _stack_pop();
private:

    void _jmp(const Instruction &ins, const std::vector<Byte>& data);

    void _ldx(const Instruction& ins, const std::vector<Byte>& data);

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
};


#endif //FCEMU_CPU_H
