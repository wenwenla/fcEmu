//
// Created by DELL on 2019/8/30.
//

#ifndef FCEMU_INSTRUCTION_H
#define FCEMU_INSTRUCTION_H

#include <string>

enum class InsType {
    IMM = 0,
    ZP = 1,
    ZPX = 2,
    ZPY = 3,
    IZX = 4,
    IZY = 5,
    ABS = 6,
    ABX = 7,
    ABY = 8,
    IND = 9,
    REL = 10,
    IMP = 11,
};

class Instruction {
public:

    const static Instruction INS[256];

    Instruction(const char* name, int cycle, InsType type, int cycle_type);

    std::string getName() const;
    int getCycle() const;
    InsType getType() const;
    int length() const;

private:
    char _name[4];
    int _cycle;
    InsType _type;
    int _cycle_type;
};

#endif //FCEMU_INSTRUCTION_H
