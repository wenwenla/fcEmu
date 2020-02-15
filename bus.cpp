#include "bus.h"
#include "chip.h"
#include <cassert>

uint8_t Bus::read(uint16_t addr) {
    // printf("Read $%04X\n", addr);
    for (auto chip : m_chips) {
        if (chip->sensitive(addr)) {
            return chip->read(addr);
        }
    }
    assert (!"CANNOT FIND CHIP");
    return -1;
}

std::vector<Byte> Bus::read(uint16_t addr, int len) {
    std::vector<Byte> res;
    for (int i = 0; i < len; ++i) {
        res.push_back(read(addr + i));
    }
    return res;
}

void Bus::write(uint16_t addr, uint8_t data) {
    for (auto chip : m_chips) {
        if (chip->sensitive(addr)) {
            chip->write(addr, data);
            return;
        }
    }
    assert (!"CANNOT FIND CHIP");
}

void Bus::connect(PtrToChip chip) {
    m_chips.push_back(chip);
}