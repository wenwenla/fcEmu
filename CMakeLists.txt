cmake_minimum_required(VERSION 3.14)
project(fcEmu)

set(CMAKE_CXX_STANDARD 14)

add_executable(fcEmu main.cpp rom/RomLoader.cpp rom/RomLoader.h utility.h rom/RomData.cpp rom/RomData.h memory/Memory.cpp memory/Memory.h cpu/Cpu.cpp cpu/Cpu.h utility.cpp cpu/Instruction.cpp cpu/Instruction.h)