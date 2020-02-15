#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>
#include <vector>
#include <SFML/Graphics/Color.hpp>

using Byte = unsigned char;

Byte low(unsigned short byte2);
Byte high(unsigned short byte2);
uint16_t combine(Byte low, Byte high);

extern sf::Color PALETTES[64];

#endif //FCEMU_UTILITY_H
