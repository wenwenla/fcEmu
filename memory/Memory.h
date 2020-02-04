#ifndef FCEMU_MEMORY_H
#define FCEMU_MEMORY_H

#include "../utility.h"
#include "../rom/RomData.h"
#include <vector>

class Memory {
public:
    Byte read(int addr) const;
    std::vector<Byte> read(int addr, int len) const;
    void write(int addr, Byte data);
    void write(int addr, const std::vector<Byte>& data);
    void loadRom(const RomData& rom);
private:
    Byte _mem[0xffff];
};


#endif //FCEMU_MEMORY_H
