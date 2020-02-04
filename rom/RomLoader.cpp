#include "RomLoader.h"
#include <fstream>

using std::ifstream;

RomData RomLoader::operator()(const char *fileName) {
    std::vector<Byte> data;
    ifstream in(fileName);
    if(!in.is_open()) {
        std::string error_msg("Can not open file ");
        throw std::runtime_error(error_msg + fileName);
    }

    in.seekg(0, std::ios::end);
    int length = in.tellg();
    in.seekg(0, std::ios::beg);
    char* byte_array = new char[length];
    in.read(byte_array, length);
    data.resize(length);
    for(int i = 0; i < length; ++i) {
        data[i] = byte_array[i];
    }
    delete[] byte_array;
    return RomData(data);
}
