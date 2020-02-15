#include "chip.h"
#include "bus.h"
#include <cassert>
#include <SFML/Window/Keyboard.hpp>

void PGRRom::load(const std::vector<Byte>& content) {
    assert(content.size() == 0x4000 or content.size() == 0x8000);
    m_data = content;
}

uint8_t PGRRom::read(uint16_t addr) {
    addr -= 0x8000;
    if (m_data.size() == 0x4000) {
        addr &= 0x3FFF;
    } else {
        addr &= 0x7FFF;
    }
    return m_data[addr];
}

void PGRRom::write(uint16_t addr, uint8_t data) {
    assert(!"CANNOT WRITE PGRROM");
}

bool PGRRom::sensitive(uint16_t addr) {
    return addr >= 0x8000 && addr < 0x8000 + 0x4000 * 2;
}

Ram::Ram() {
    m_data.resize(0x800);
}

uint8_t Ram::read(uint16_t addr) {
    assert (addr >= 0 && addr < 0x2000);
    addr %= 0x800;
    return m_data[addr];
}

void Ram::write(uint16_t addr, uint8_t data) {
    assert (addr >= 0 && addr < 0x2000);
    m_data[addr % 0x800] = data;
}

bool Ram::sensitive(uint16_t addr) {
    return addr >= 0 && addr < 0x2000;
}

InputRegister::InputRegister() : m_btn{0}, m_input_pos_1{0} {}

bool InputRegister::sensitive(uint16_t addr) {
    return addr == 0x4016 || addr == 0x4017;
}

uint8_t InputRegister::read(uint16_t addr) {
    // printf("Read from Input: $%04X\n", addr);
    if (addr == 0x4016) {
        if (m_input_pos_1 == 8) return 1;
        int res = m_btn[m_input_pos_1];
        m_input_pos_1 += 1;
        return res;
    } else {
        //TODO
        return 0;
    }
}

void InputRegister::write(uint16_t addr, uint8_t data) {
    // printf("Write To Input: $%04X = %d\n", addr, data);
    if (addr == 0x4016) {
        if (data & 1) {
            m_input_pos_1 = 0;
            m_update_state = 1;
        } else {
            updateKeyState(); // The function cannot be use on remote computers.
            m_update_state = 0;
        }
    } else {
        //TODO
    }
}

void InputRegister::updateKeyState() {
    sf::Keyboard::Key key_map[8] = {
        sf::Keyboard::Z, //A
        sf::Keyboard::X, //B
        sf::Keyboard::Space, //select
        sf::Keyboard::Enter, //start
        sf::Keyboard::Up, //up
        sf::Keyboard::Down, //down
        sf::Keyboard::Left, //left
        sf::Keyboard::Right //right
    };
    if (m_update_state == 1) {
        for (int i = 0; i < 8; ++i) {
            m_btn[i] = sf::Keyboard::isKeyPressed(key_map[i]);
        }
    }
}

PPUReg::PPUReg() {
    m_sprites.resize(256);
}

uint8_t PPUReg::read(uint16_t addr) {
    if (addr == 0x4014) {
        assert(!"TODO: CANNOT READ 0x4014");
    }
    addr = (addr - 0x2000) % 8;
    uint8_t res = 0;
    switch (addr) {
    case 0:
        assert(!"CANNOT READ 0x2000");
        break;
    case 1:
        assert(!"CANNOT READ 0x2001");
        break;
    case 2:
        m_status &= 224; //1110 0000
        res = m_status;
        m_status &= 96; //0110 0000
        m_addr_latch = 0;
        break;
    case 3:
        assert(!"CANNOT READ 0x2003");
        break;
    case 4:
        res = m_sprites[m_sprites_addr++];
        break;
    case 5:
        assert(!"CANNOT READ 0x2005");
        break;
    case 6:
        assert(!"CANNOT READ 0x2006");
        break;
    case 7:
        res = m_buffered_data;
        m_buffered_data = m_ppu_bus->read(m_ppu_addr);
        if (m_ppu_addr >= 0x3F00 && m_ppu_addr <= 0x3FFF) {
            res = m_buffered_data;
        }
        if (m_ctrl >> 2 & 1) {
            m_ppu_addr += 32;
        } else {
            m_ppu_addr += 1;
        }
        break;
    }
    return res;
}
 
void PPUReg::write(uint16_t addr, uint8_t data) {
    if (addr == 0x4014) {
        //copy data from 0x??00 -- 0x??FF to sprites
        m_dma = true;
        m_dma_addr = (data << 8);
        return;
    }
    addr = (addr - 0x2000) % 8;
    switch (addr) {
    case 0:
        m_ctrl = data;
        // printf("ctrl = %d\n", data);
        break;
    case 1:
        m_mask = data;
        break;
    case 2:
        assert(!"CANNOT WRITE 0x2002");
        break;
    case 3:
        m_sprites_addr = data;
        break;
    case 4:
        m_sprites[m_sprites_addr++] = data;
        break;
    case 5:
        if (!m_scroll_index) {
            m_offset_x = data;
        } else {
            m_offset_y = data;
        }
        m_scroll_index = !m_scroll_index;
        break;
    case 6:
        if (m_addr_latch == 0) {
            m_ppu_addr = ((data & 63) << 8);
            m_addr_latch = 1;
        } else {
            m_ppu_addr |= data;
            m_addr_latch = 0;
        }
        break;
    case 7:
        m_ppu_bus->write(m_ppu_addr, data);
        // printf("PPU $%04X = %d\n", m_ppu_addr, data);
        if (m_ctrl >> 2 & 1) {
            m_ppu_addr += 32;
        } else {
            m_ppu_addr += 1;
        }
        break;
    }
}

bool PPUReg::sensitive(uint16_t addr) {
    return addr >= 0x2000 && addr < 0x4000 || addr == 0x4014;
}
    
void PPUReg::connect(PtrToBus ppu_bus) {
    m_ppu_bus = ppu_bus;
}

bool PPUReg::onDMA(uint16_t& addr) {
    if (m_dma) {
        addr = m_dma_addr;
        m_dma_addr += 1;
        if ((m_dma_addr & 0xFF) == 0) {
            m_dma = false;
        }
        return true;
    }
    return false;
}

uint8_t PatternTable::read(uint16_t addr) {
    return m_data[addr];
}

void PatternTable::write(uint16_t addr, uint8_t data) {
    assert(!"Cannot write Pattern Table");
}

bool PatternTable::sensitive(uint16_t addr) {
    return addr >= 0 && addr < 0x2000;
}

void PatternTable::load(const std::vector<Byte> content) {
    assert(content.size() == 0x2000);
    m_data = content;
}


NameTable::NameTable() {
    m_data.resize(0x3F00 - 0x2000);
}

uint8_t NameTable::read(uint16_t addr) {
    return m_data[addr - 0x2000];
}

void NameTable::write(uint16_t addr, uint8_t data) {
    m_data[addr - 0x2000] = data;
}

bool NameTable::sensitive(uint16_t addr) {
    return addr >= 0x2000 && addr < 0x3F00;
}


PaletteTable::PaletteTable() {
    m_data.resize(0x20);
}

uint8_t PaletteTable::read(uint16_t addr) {
    addr &= 0x1F;
    switch (addr) {
        case 0x10: addr = 0x00; break;
        case 0x14: addr = 0x04; break;
        case 0x18: addr = 0x08; break;
        case 0x1c: addr = 0x0c; break;
    }
    return m_data[addr];
}

void PaletteTable::write(uint16_t addr, uint8_t data) {
    addr &= 0x1F;
    switch (addr) {
        case 0x10: addr = 0x00; break;
        case 0x14: addr = 0x04; break;
        case 0x18: addr = 0x08; break;
        case 0x1c: addr = 0x0c; break;
    }
    m_data[addr] = data;
}

bool PaletteTable::sensitive(uint16_t addr) {
    return addr >= 0x3F00 && addr < 0x4000;
}