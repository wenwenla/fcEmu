//
// Created by DELL on 2019/8/23.
//

#ifndef FCEMU_ROMDATA_H
#define FCEMU_ROMDATA_H

#include <vector>
#include "../utility.h"

class RomData {
public:
    RomData(const std::vector<Byte>& data);
    //TODO: flag decode
    Byte getMapperNumber() const;
    bool fourScreenMode() const;
    bool isVertical() const;
    bool hasTrainer() const;

    unsigned getPrgSize() const;
    unsigned getChrSize() const;
    const std::vector<Byte>& getPrgRom() const;
private:
    unsigned _prg_size;
    unsigned _chr_size;
    Byte _flag6, _flag7;
    std::vector<Byte> _trainer_area;
    std::vector<Byte> _prg_rom;
    std::vector<Byte> _chr_rom;
    std::vector<Byte> _miscellaneous_rom;

private:
    void _initMem(const std::vector<Byte> &data);
};


#endif //FCEMU_ROMDATA_H
