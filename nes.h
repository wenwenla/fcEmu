#ifndef _NES_H_
#define _NES_H_

#include <vector>
#include "utility.h"

class Nes {
public:
    void load(const char* fileName);
    const std::vector<Byte>& getPgr() const;
    const std::vector<Byte>& getChr() const;
private:
    int m_mapper;
    std::vector<Byte> m_trainer;
    std::vector<Byte> m_pgr;
    std::vector<Byte> m_chr;
};

#endif