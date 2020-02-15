#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "bus.h"
#include "chip.h"
#include "cpu.h"
#include "entity.h"
#include "ppu.h"

class Machine {
public:
    Machine();
    void step(int64_t ns);

    const std::shared_ptr<Cpu> getCpu() const { return m_cpu; }
    const std::shared_ptr<Ppu> getPpu() const { return m_ppu; }
private:
    std::shared_ptr<Bus> m_cpu_bus, m_ppu_bus;
    std::shared_ptr<Cpu> m_cpu {nullptr};
    std::shared_ptr<Ppu> m_ppu {nullptr};
    std::shared_ptr<InputRegister> m_input {nullptr};
    int64_t m_time_last;
};

class MachineEntity : public Entity {
public:
    MachineEntity() {
        machine = new Machine;
        m_font = new sf::Font;
        m_font->loadFromFile("resource/consola.ttf");
        m_code = machine->getCpu()->asmCode(0xC000, 0xFFF0);
        text.setFont(*m_font);

        m_palettes.create(16, 2);
        m_background.create(256, 240);
     }

    ~MachineEntity() {
        delete machine;
        delete m_font;
    }

    void onUpdate(int ms) override {
        long long ns = ms * 1000000LL;
        sf::Clock c;
        c.restart();
        machine->step(ns);
        printf("T: %d ms TC: %d ms\n", ms, c.restart().asMilliseconds());
    }

    void onRender(sf::RenderWindow& window) override {
        drawCode(window);
        drawPalettes(window);
        drawBackground(window);
    }

    void drawCode(sf::RenderWindow& window) {
        auto cpu = machine->getCpu();
        auto pc = cpu->getPc();
        auto it = m_code.lower_bound(pc);
        if (it == m_code.end()) {
            printf("CANNOT FIND ADDR\n");
            return;
        }
        for (int i = 0; i < 5; ++i) {
            --it;
            if (it == m_code.begin()) break;
        }
        for (int i = 0; i < 10; ++i) {
            if (it == m_code.end()) break;
            auto addr = it->first;
            auto code = it->second;
            ++it;
            text.setPosition(0, 30 + i * 30);
            text.setString(code);
            if (addr == pc) {
                text.setFillColor(sf::Color::Red);
            } else {
                text.setFillColor(sf::Color::Black);
            }
            window.draw(text);
        }
    }

    void drawPalettes(sf::RenderWindow& window) {
        auto ppu = machine->getPpu();
        auto palettes = ppu->getPalettes();
        for (int i = 0; i < 16; ++i) {
            m_palettes.setPixel(i, 0, palettes[i]);
            m_palettes.setPixel(i, 1, palettes[i + 16]);
        }
        sf::Texture texture;
        bool ret = texture.loadFromImage(m_palettes);
        sf::Sprite sprite(texture);
        sprite.setScale(16, 16);
        auto bound = sprite.getGlobalBounds();
        auto winsize = window.getSize();
        sprite.setPosition(0, winsize.y - bound.height);
        window.draw(sprite);
    }

    void drawBackground(sf::RenderWindow& window) {
        auto ppu = machine->getPpu();
        auto bg = ppu->getBackground(0);
        for (unsigned i = 0; i < bg.size(); ++i) {
            m_background.setPixel(i % 256, i / 256, PALETTES[bg[i]]);
        }
        drawSprites();
        sf::Texture texture;
        bool ret = texture.loadFromImage(m_background);
        sf::Sprite sprite(texture);
        sprite.setScale(2, 2);
        auto bound = sprite.getGlobalBounds();
        auto winsize = window.getSize();
        sprite.setPosition(winsize.x - bound.width, winsize.y - bound.height);
        window.draw(sprite);
    }

    void drawSprites() {
        auto ppu = machine->getPpu();
        auto sprites = ppu->getPpuSprites();
        for (int i = 63; i >= 0; --i) {
            auto sprite = sprites[i];
            for (int pixel = 0; pixel < 64; ++pixel) {
                if (sprite.x + pixel % 8 >= 256 || sprite.y + pixel / 8 + 1 >= 240) continue;
                m_background.setPixel(
                    sprite.x + pixel % 8, 
                    sprite.y + pixel / 8 + 1, 
                    PALETTES[sprite.color_index[pixel]]
                );
            }
        }
    }
private:
    Machine* machine{nullptr};
    sf::Font* m_font{nullptr};
    std::map<uint16_t, std::string> m_code;
    sf::Text text;

    sf::Image m_palettes;
    sf::Image m_background;
};

#endif