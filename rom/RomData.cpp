#include "RomData.h"
#include <cassert>


void RomData::_initMem(const std::vector<Byte>& data) {
    int start = 16;
    if(hasTrainer()) {
        _trainer_area.resize(512);
        for(int i = 0; i < 512; ++i) {
            _trainer_area[i] = data[start++];
        }
    }
    _prg_rom.resize(_prg_size);
    for(int i = 0; i < _prg_size; ++i) {
        _prg_rom[i] = data[start++];
    }
    _chr_rom.resize(_chr_size);
    for(int i = 0; i < _chr_size; ++i) {
        _chr_rom[i] = data[start++];
    }
    while(start < data.size()) {
        _miscellaneous_rom.push_back(data[start++]);
    }
}

RomData::RomData(const std::vector<Byte> &data) {
    assert(data[0] == 'N' && data[1] == 'E' && data[2] == 'S' && data[3] == 0x1a);
    _prg_size = data[4] * 16 * 1024;
    _chr_size = data[5] * 8 * 1024;
    _flag6 = data[6];
    _flag7 = data[7];
    _initMem(data);
}

bool RomData::hasTrainer() const {
    return _flag6 >> 2 & 1;
}

unsigned RomData::getPrgSize() const {
    return _prg_size;
}

unsigned RomData::getChrSize() const {
    return _chr_size;
}

const std::vector<Byte> &RomData::getPrgRom() const {
    return _prg_rom;
}

