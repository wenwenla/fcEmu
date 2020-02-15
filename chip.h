#ifndef _CHIP_H_
#define _CHIP_H_

#include <cstdint>
#include <vector>
#include <memory>
#include "utility.h"

class Bus;
using PtrToBus = std::shared_ptr<Bus>;

class Chip {
public:
    virtual ~Chip() {}
    virtual bool sensitive(uint16_t addr) { return false; }
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t data) = 0;
};

using PtrToChip = std::shared_ptr<Chip>;

class PGRRom : public Chip {
public:
    void load(const std::vector<Byte>& content);
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    bool sensitive(uint16_t addr) override;
private:
    std::vector<Byte> m_data;
};

class Ram : public Chip {
public:
    Ram();
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    bool sensitive(uint16_t addr) override;
private:
    std::vector<Byte> m_data;
};

class PPUReg : public Chip {
public:
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    bool sensitive(uint16_t addr) override;
    void connect(PtrToBus ppu_bus);

    Byte& ctrl() { return m_ctrl; }
    Byte& mask() { return m_mask; }
    Byte& status() { return m_status; }

private:
    Byte m_ctrl = 0;
    Byte m_mask = 0;
    Byte m_status = 0;
    Byte m_addr_x = 0;
    Byte m_data_x = 0;
    Byte m_scroll = 0;
    Byte m_addr = 0;
    Byte m_data = 0;

    Byte m_buffered_data = 0;
    uint16_t m_ppu_addr = 0;
    int m_addr_latch = 0;

    PtrToBus m_ppu_bus{nullptr};
};

class InputRegister : public Chip {
public:
    enum Key {
        A = 0,
        B = 1,
        SELECT = 2,
        START = 3,
        UP = 4,
        DOWN = 5,
        LEFT = 6,
        RIGHT = 7
    };
    InputRegister();
    bool sensitive(uint16_t addr) override;
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    void updateKeyState();
private:
    bool m_btn[8]; // A B SELECT START UP DOWN LEFT RIGHT
    int m_input_pos_1;
    int m_update_state = 0;
    //player2 is obmit
};

class HackChip : public Chip {
public:
    bool sensitive(uint16_t addr) override {
        return addr >= 0x4020 && addr < 0x8000;
    }

    uint8_t read(uint16_t addr) override {
        return 0;
    }

    void write(uint16_t addr, uint8_t data) override {

    }
};

class PatternTable : public Chip {
public:
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    bool sensitive(uint16_t addr) override;
    void load(const std::vector<Byte> content);

private:
    std::vector<Byte> m_data;
};

class NameTable : public Chip {
public:
    NameTable();
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    bool sensitive(uint16_t addr) override;
private:
    std::vector<Byte> m_data;
};

class PaletteTable : public Chip {
public:
    PaletteTable();
    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t data) override;
    bool sensitive(uint16_t addr) override;
private:
    std::vector<Byte> m_data;
};

#endif