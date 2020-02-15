#ifndef _PPU_H_
#define _PPU_H_

#include "utility.h"
#include <memory>
#include <vector>
#include <SFML/Graphics/Color.hpp>

class Bus;
using PtrToBus = std::shared_ptr<Bus>; 
class Chip;
using PtrToChip = std::shared_ptr<Chip>;
class PPUReg;
using PtrToPPUReg = std::shared_ptr<PPUReg>;

class Ppu {
public:
    using ColorPalettes = std::vector<sf::Color>;
    using ColorBG = std::vector<Byte>;
    Ppu(PtrToBus bus);
    void run();
    PtrToChip getRegister() const;
    bool& nmi() { return m_nmi; }

    ColorPalettes getPalettes() const;
    ColorBG getBackground(int index) const;

    std::vector<Byte> getSprite(int sprite_table, int index) const;
private:
    int m_row;
    int m_col;
    bool m_nmi;

    PtrToBus m_bus;
    PtrToPPUReg m_register;
};

#endif