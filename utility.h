//
// Created by DELL on 2019/8/23.
//

#ifndef FCEMU_UTILITY_H
#define FCEMU_UTILITY_H

#include <string>
#include <vector>

using Byte = unsigned char;

Byte low(unsigned short byte2);
Byte high(unsigned short byte2);
unsigned short combine(Byte low, Byte high);

#endif //FCEMU_UTILITY_H
