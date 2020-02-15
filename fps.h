#ifndef _FPS_H_
#define _FPS_H_

#include "entity.h"

class Fps : public Entity {
public:
    Fps();
    ~Fps();
    void onRender(sf::RenderWindow& window) override;

private:
    sf::Clock* m_clock;
    sf::Font* m_font;
    int m_count;
    double m_fps;
};

#endif