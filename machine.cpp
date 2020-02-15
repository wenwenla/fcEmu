#include "machine.h"
#include "nes.h"
#include <iostream>


Machine::Machine() {
    auto nes = Nes();
    nes.load("nesrom/dajingang.nes");

    m_ppu_bus = std::make_shared<Bus>();
    m_ppu = std::make_shared<Ppu>(m_ppu_bus);
    auto ppu_pattern = std::make_shared<PatternTable>();
    ppu_pattern->load(nes.getChr());
    auto ppu_name = std::make_shared<NameTable>();
    auto ppu_palette = std::make_shared<PaletteTable>();
    m_ppu_bus->connect(ppu_pattern);
    m_ppu_bus->connect(ppu_name);
    m_ppu_bus->connect(ppu_palette);

    m_cpu_bus = std::make_shared<Bus>();
    auto cpu_ram = std::make_shared<Ram>();
    auto cpu_hack = std::make_shared<HackChip>();
    auto pgr_rom = std::make_shared<PGRRom>();
    m_input = std::make_shared<InputRegister>();
    pgr_rom->load(nes.getPgr());
    m_cpu_bus->connect(cpu_ram);
    m_cpu_bus->connect(m_ppu->getRegister());
    m_cpu_bus->connect(pgr_rom);
    m_cpu_bus->connect(m_input);
    m_cpu_bus->connect(cpu_hack);
    m_cpu = std::make_shared<Cpu>(m_cpu_bus);
    m_cpu->setRST();
    m_time_last = 0;
}

void Machine::step(int64_t ns) {
    m_time_last += ns;
    int cnt = 0;
    while (m_time_last > 0) {
        int cycles = m_cpu->run();
        
        //CHECK DMA
        uint16_t addr;
        while (m_ppu->onDMA(addr)) {
            m_ppu->transferData(addr, m_cpu_bus->read(addr));
        }
        //DMA END
        m_time_last -= cycles * 601;
        
        for (int t = 0; t < cycles; ++t) {
            m_ppu->run();
            m_ppu->run();
            m_ppu->run();
            if (m_ppu->nmi()) {
                m_cpu->setNMI();
                m_ppu->nmi() = false;
            }
        }

    }
}