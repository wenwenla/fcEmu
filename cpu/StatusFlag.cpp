//
// Created by DELL on 2019/8/30.
//

#include "StatusFlag.h"
#include <cassert>

void StatusFlag::reset() {
    _flag.reset();
}

void StatusFlag::setNegative(int flag) {
    assert(flag == 0 || flag == 1);
    _flag.set(7, flag);
}

void StatusFlag::setOverflow(int flag) {
    assert(flag == 0 || flag == 1);
    _flag.set(6, flag);
}

void StatusFlag::setBreak(int flag) {
    assert(flag == 0 || flag == 1);
    _flag.set(4, flag);
}

void StatusFlag::setDecimal(int flag) {
    assert(flag == 0 || flag == 1);
    _flag.set(3, flag);
}

void StatusFlag::setIntDisable(int flag) {
    assert(flag == 0 || flag == 1);
    _flag.set(2, flag);
}

void StatusFlag::setZero(int flag) {
    assert(flag == 0 || flag == 1);
    _flag.set(1, flag);
}

void StatusFlag::setCarry(int flag) {
    assert(flag == 0 || flag == 1);
    _flag.set(0, flag);
}

StatusFlag::StatusFlag() {
    _flag.reset();
    _flag.set(5, 1);
}

int StatusFlag::getNegative() const {
    return _flag[7];
}

int StatusFlag::getOverflow() const {
    return _flag[6];
}

int StatusFlag::getBreak() const {
    return _flag[4];
}

int StatusFlag::getDecimal() const {
    return _flag[3];
}

int StatusFlag::getIntDisable() const {
    return _flag[2];
}

int StatusFlag::getZero() const {
    return _flag[1];
}

int StatusFlag::getCarry() const {
    return _flag[0];
}
