#ifndef _PPU_H_
#define _PPU_H_

#include "utility.h"
#include <memory>
#include <vector>
#include <array>
#include <SFML/Graphics/Color.hpp>

class Bus;
using PtrToBus = std::shared_ptr<Bus>; 
class Chip;
using PtrToChip = std::shared_ptr<Chip>;
class PPUReg;
using PtrToPPUReg = std::shared_ptr<PPUReg>;

struct PpuSprite {
    std::vector<Byte> color_index;
    bool h_flip{false}, v_flip{false};
    Byte x{0}, y{0}, z{0};
};

class Ppu {
public:
    using ColorPalettes = std::vector<sf::Color>;
    using ColorBGIndex = std::vector<Byte>;
    using Sprites = std::array<PpuSprite, 64>;
    Ppu(PtrToBus bus);
    void run();
    PtrToChip getRegister() const;
    bool& nmi() { return m_nmi; }

    ColorPalettes getPalettes() const;
    std::vector<Byte> getPattern(int sprite_table, int index) const;
    
    const ColorBGIndex& getBackground(int index);
    const Sprites& getPpuSprites();

    Byte getBgColorIndex(int pixel_row, int pixel_col);

    bool onDMA(uint16_t& addr);
    void transferData(uint16_t addr, Byte data);
private:
    int m_row;
    int m_col;
    bool m_nmi;

    PtrToBus m_bus;
    PtrToPPUReg m_register;

    ColorBGIndex m_bg;
    Sprites m_sprites;
};

#endif