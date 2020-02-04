#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "utility.h"

Byte low(unsigned short byte2) {
    return byte2 & ((1 << 8) - 1);
}

Byte high(unsigned short byte2) {
    return byte2 >> 8;
}

unsigned short combine(Byte low, Byte high) {
    return low + high * 256;
}