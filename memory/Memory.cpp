#include "Memory.h"
#include <cassert>
#include <iostream>

Byte Memory::read(int addr) const {
    Byte res;
    if(addr < 0x2000) {
        res = _mem[addr % 0x800];
    } else if(addr < 0x4000) {
        res =  _mem[(addr - 0x2000) % 8 + 0x2000];
    } else {
        res = _mem[addr];
    }
    return res;
}

std::vector<Byte> Memory::read(int addr, int len) const {
    std::vector<Byte> result;
    result.reserve(len);
    for(int i = 0; i < len; ++i) {
        result.push_back(read(addr + i));
    }
    return result;
}

void Memory::write(int addr, Byte data) {
    if(addr < 0x2000) {
        _mem[addr % 0x800] = data;
    } else if(addr < 0x4000) {
        _mem[(addr - 0x2000) % 8 + 0x2000] = data;
    } else if(addr < 0x4020) {
        _mem[addr] = data;
    } else if(addr < 0x6000) {
        // write ROM
        assert(false);
    } else if(addr < 0x8000) {
        _mem[addr] = data;
    } else {
        // write ROM
        assert(false);
    }
}

void Memory::write(int addr, const std::vector<Byte> &data) {
    for(int i = 0; i < data.size(); ++i) {
        write(addr + i, data[i]);
    }
}

void Memory::loadRom(const RomData &rom) {
    auto prg_rom = rom.getPrgRom();
    if(rom.getPrgSize() == 16 * 1024) {
        for(int i = 0; i < 16 * 1024; ++i) {
            _mem[0x8000 + i] = prg_rom[i];
            _mem[0xc000 + i] = prg_rom[i];
        }
    } else if(rom.getPrgSize() == 32 * 1024) {
        for(int i = 0; i < 32 * 1024; ++i) {
            _mem[0x8000 + i] = prg_rom[i];
        }
    } else {
        //Unsupported rom
        std::cerr << "Unsupported Rom Size " << rom.getPrgSize() << std::endl;
        assert(false);
    }
}
