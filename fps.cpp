#include "fps.h"
#include <cstdio>

Fps::Fps() {
    m_clock = new sf::Clock;
    m_clock->restart();
    m_count = 0;
    m_fps = 0;
    m_font = new sf::Font;
    m_font->loadFromFile("./resource/consola.ttf");
}

Fps::~Fps() {
    delete m_clock;
    m_clock = nullptr;
}

void Fps::onRender(sf::RenderWindow& window) {
    ++m_count;
    if (m_count == 10) {
        m_count = 0;
        auto t = m_clock->restart();
        m_fps = 10 / t.asSeconds();
    }
    char s[32] = {0};
    sprintf(s, "FPS: %.2f", m_fps);
    puts(s);
    sf::Text text(s, *m_font);
    text.setPosition(0, 0);
    text.setFillColor(sf::Color::Red);
    window.draw(text);
}