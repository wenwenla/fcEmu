#ifndef FCEMU_ROMLOADER_H
#define FCEMU_ROMLOADER_H

#include "../utility.h"
#include "RomData.h"

class RomLoader {
public:
    RomData operator()(const char* fileName);
};


#endif //FCEMU_ROMLOADER_H
