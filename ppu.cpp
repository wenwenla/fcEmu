#include "ppu.h"
#include "bus.h"
#include "chip.h"
#include "utility.h"

Ppu::Ppu(PtrToBus bus) : m_row{0}, m_col{0}, m_bus(bus) {
    m_register = std::make_shared<PPUReg>();
    m_register->connect(bus);
    m_nmi = false;
}

void Ppu::run() {
    ++m_col;
    if (m_col == 340) {
        ++m_row;
        m_col = 0;
    }
    if (m_row == 261) {
        m_row = 0;
        m_col = 0;
        m_register->status() &= 0xEF;
    }
    if (m_row == 240 && m_col == 0) {
        m_register->status() |= (1 << 7);
        if (m_register->ctrl() >> 7 & 1) {
            m_nmi = true;
        }
    }
}

PtrToChip Ppu::getRegister() const {
    return m_register;
}

Ppu::ColorPalettes Ppu::getPalettes() const {
    ColorPalettes result;
    result.resize(32);
    for (int i = 0; i < 32; ++i) {
        int index = m_bus->read(0x3F00 + i);
        result[i] = PALETTES[index];
    }
    return result;
}

Ppu::ColorBG Ppu::getBackground(int index) const {
    int addr_map[] = {0x2000, 0x2400, 0x2800, 0x2c00};
    int start_addr = addr_map[index];
    int sprite_table = (m_register->ctrl() >> 4 & 1);
    ColorBG result(256 * 240);
    for (int row = 0; row < 30; ++row) {
        for (int col = 0; col < 32; ++col) {
            //load attribute bits
            int attr_addr = addr_map[index] + 960 + col / 4 + row / 4 * 8;
            Byte attr_byte = m_bus->read(attr_addr);
            int attr;
            // int tmp_r = row % 4, tmp_c = col % 4;
            int magic = (((row & 3) >> 1) << 1) + ((col & 3) >> 1);
            magic <<= 1;
            attr = (attr_byte >> magic & 3);
            // if (tmp_r < 2 && tmp_c < 2) {
            //     attr = (attr_byte >> 0 & 3);
            // } else if (tmp_r < 2 && tmp_c >= 2) {
            //     attr = (attr_byte >> 2 & 3);
            // } else if (tmp_r >= 2 && tmp_c < 2) {
            //     attr = (attr_byte >> 4 & 3);
            // } else {
            //     attr = (attr_byte >> 6 & 3);
            // }
            //load end

            int sprite_index = m_bus->read(start_addr++);
            auto bg = getSprite(sprite_table, sprite_index);
            for (int k = 0; k < 64; ++k) {
                int r = k / 8, c = k % 8;
                int pixel_r = row * 8 + r, pixel_c = col * 8 + c;
                int color_index = 0x3F00;
                if (bg[k]) {
                    color_index += (attr << 2 | bg[k]); 
                }
                result[pixel_r * 256 + pixel_c] = m_bus->read(color_index);
            }
        }
    }
    return result;
}

std::vector<Byte> Ppu::getSprite(int sprite_table, int index) const {
    int addr = (sprite_table << 12) + index * 16;
    std::vector<Byte> res;
    for (int i = 0; i < 8; ++i) {
        int l = m_bus->read(addr + i);
        int h = m_bus->read(addr + 8 + i);
        for (int j = 7; j >= 0; --j) {
            int this_index = (l >> j & 1) | (((h >> j) & 1) << 1);
            res.push_back(this_index);
        }
    }
    return res;
}