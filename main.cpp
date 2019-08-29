#include <iostream>
#include "rom/RomLoader.h"
#include "memory/Memory.h"

int main() {
    auto loader = RomLoader();
    auto rom = loader("../nesrom/nestest.nes");
    printf("PRG size: %d\n", rom.getPrgSize());
    auto mem = Memory();
    mem.loadRom(rom);
    printf("RESET: 0x%x\n", mem.read(0xfffd) * 256 + mem.read(0xfffc));
    printf("NMI: 0x%x\n", mem.read(0xfffb) * 256 + mem.read(0xfffa));
    printf("IRQ/BRK: 0x%x\n", mem.read(0xffff) * 256 + mem.read(0xfffe));
    printf("OP CODE: 0x%x\n", mem.read(0xc004));
    return 0;
}