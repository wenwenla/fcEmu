#ifndef _GAME_H_
#define _GAME_H_

#include "entity.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Game {
public:
    Game(int width, int height, const char* name);
    ~Game();

    void run();

    void addEntity(PtrToEntiry entity);

private:

    void _processEvent();
    void _update();
    void _render();

private:
    sf::RenderWindow* m_window = nullptr;
    std::vector<PtrToEntiry> m_entity;

    sf::Clock* m_clock = nullptr;
};

#endif