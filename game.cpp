#include "game.h"

Game::Game(int width, int height, const char* name) {
    m_window = new sf::RenderWindow(sf::VideoMode(width, height), name);
    m_clock = new sf::Clock();
    m_clock->restart();

    //m_window->setFramerateLimit(60);
}

Game::~Game() {
    delete m_window;
    m_window = nullptr;
}

void Game::run() {
    while(m_window->isOpen()) {
        _processEvent();
        _update();
        _render();
    }
}

void Game::addEntity(PtrToEntiry entity) {
    m_entity.push_back(entity);
}

void Game::_processEvent() {
    sf::Event event;
    while (m_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window->close();
        }
        for (auto entity : m_entity) {
            entity->onEvent(event);
        }
    }
}

void Game::_update() {
    auto t = m_clock->restart();
    for (auto entity : m_entity) {
        entity->onUpdate(t.asMilliseconds());
    }
}

void Game::_render() {
    m_window->clear(sf::Color::White);
    for (auto entity : m_entity) {
        entity->onRender(*m_window);
    }
    m_window->display();
}