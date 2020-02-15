#ifndef _BUS_H_
#define _BUS_H_

#include <cstdint>
#include <vector>
#include <memory>
#include "utility.h"

class Chip;
using PtrToChip = std::shared_ptr<Chip>;

class Bus {
public:
    Byte read(uint16_t addr);
    std::vector<Byte> read(uint16_t addr, int len);
    void write(uint16_t addr, uint8_t data);
    void connect(PtrToChip chip);
private:
    std::vector<PtrToChip> m_chips;
};

using PtrToBus = std::shared_ptr<Bus>; 

#endif