#include "nes.h"
#include <fstream>


void Nes::load(const char* fileName) {
    using std::ifstream;
    ifstream fin;
    fin.open(fileName, std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        throw std::runtime_error(std::string("Cannot open file: ") + fileName);
    }
    fin.seekg(0, fin.end);
    int length = fin.tellg();
    fin.seekg(0, fin.beg);
    std::vector<char> buf(length);
    fin.read(buf.data(), length);
    fin.close();
    if (buf[0] != 'N' || buf[1] != 'E' || buf[2] != 'S') {
        throw std::runtime_error(std::string("Invalid nes rom"));
    }
    char flag6 = buf[6];
    char flag7 = buf[7];
    m_mapper = (flag6 & 0xF0 << 4) | (flag7 & 0xF0);
    printf("PGR size: %d bytes\n", buf[4] * 16384);
    printf("CHR size: %d bytes\n", buf[5] * 8192);
    printf("Mapper: %d\n", m_mapper);
    
    int flag_f = (flag6 >> 3 & 1);
    int flag_t = (flag6 >> 2 & 1);
    int flag_b = (flag6 >> 1 & 1);
    int flag_m = (flag6 >> 0 & 1);
    int flag_p = (flag7 >> 1 & 1);
    int flag_v = (flag7 >> 0 & 1);

    int start = 16;
    if (flag_t) {
        m_trainer.resize(512);
        for (int i = 0; i < 512; ++i) {
            m_trainer[i] = buf[start];
            ++start;
        }
    }

    m_pgr.resize(buf[4] * 16384);
    for (int i = 0; i < buf[4] * 16384; ++i) {
        m_pgr[i] = buf[start];
        ++start;
        if (i < 10) {
            printf("%02x ", m_pgr[i]);
        }
    }
    puts("");

    m_chr.resize(buf[5] * 8192);
    for (int i = 0; i < buf[5] * 8192; ++i) {
        m_chr[i] = buf[start];
        ++start;
    }
}

const std::vector<Byte>& Nes::getPgr() const {
    return  m_pgr;
}

const std::vector<Byte>& Nes::getChr() const {
    return m_chr;
}