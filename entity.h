#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <SFML/Graphics.hpp>
#include <memory>

class Entity {
public:
    virtual ~Entity() {}
    virtual void onEvent(const sf::Event& event) {}
    virtual void onRender(sf::RenderWindow& window) {}
    virtual void onUpdate(int ms) {}
};

using PtrToEntiry = std::shared_ptr<Entity>;

#endif