#include <iostream>
#include <fstream>
#include "rom/RomLoader.h"
#include "memory/Memory.h"
#include "utility.h"
#include "cpu/Instruction.h"
#include "cpu/Cpu.h"

int main() {
    auto loader = RomLoader();
    auto rom = loader("../nesrom/nestest.nes");
    printf("PRG size: %d\n", rom.getPrgSize());
    auto mem = Memory();
    mem.loadRom(rom);
//    printf("RESET: 0x%x\n", mem.read(0xfffd) * 256 + mem.read(0xfffc));
//    printf("NMI: 0x%x\n", mem.read(0xfffb) * 256 + mem.read(0xfffa));
//    printf("IRQ/BRK: 0x%x\n", mem.read(0xffff) * 256 + mem.read(0xfffe));
//    printf("OP CODE: 0x%x\n", mem.read(0xc004));
//    printf("%s\n", decode({0x78}).c_str());
//    puts("==============Disassemble==================");
//    int addr = 0x8000;
//    while(addr < 0xc000) {
//        int opcode = mem.read(addr);
//        Instruction ins = Instruction::INS[opcode];
//        int length = ins.length();
//        auto data = mem.read(addr, length);
//        printf("0x%04x: %s\n", addr, decode(data).c_str());
//        addr += length;
//    }
//    puts("=================END=======================");

    std::ifstream fs("../docs/log.sp");
    std::string line;
    Cpu cpu(mem);
    for(int _ = 0; _ < 102400; ++_) {
        fs >> line;
        auto log = cpu.log();
        if(log != line) {
            std::cout << "ERROR: \n" << line << "\n" << log << std::endl;
            return -1;
        }
        cpu.run();
    }
    return 0;
}