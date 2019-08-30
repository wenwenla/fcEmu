//
// Created by DELL on 2019/8/30.
//

#include "StatusFlag.h"
#include <cassert>

StatusFlag::StatusFlag() {
    reset();
}

void StatusFlag::reset() {
    _flag.reset();
    _flag.set(5, true);
}

void StatusFlag::setNegative(bool flag) {
    _flag.set(7, flag);
}

void StatusFlag::setOverflow(bool flag) {
    _flag.set(6, flag);
}

void StatusFlag::setBreak(bool flag) {
    _flag.set(4, flag);
}

void StatusFlag::setDecimal(bool flag) {
    _flag.set(3, flag);
}

void StatusFlag::setIntDisable(bool flag) {
    _flag.set(2, flag);
}

void StatusFlag::setZero(bool flag) {
    _flag.set(1, flag);
}

void StatusFlag::setCarry(bool flag) {
    _flag.set(0, flag);
}

bool StatusFlag::getNegative() const {
    return _flag[7];
}

bool StatusFlag::getOverflow() const {
    return _flag[6];
}

bool StatusFlag::getBreak() const {
    return _flag[4];
}

bool StatusFlag::getDecimal() const {
    return _flag[3];
}

bool StatusFlag::getIntDisable() const {
    return _flag[2];
}

bool StatusFlag::getZero() const {
    return _flag[1];
}

bool StatusFlag::getCarry() const {
    return _flag[0];
}

int StatusFlag::toInt() const {
    int result = 0;
    for(int i = 7; i >= 0; --i) {
        result = result * 2 + _flag[i];
    }
    return result;
}

Byte StatusFlag::toByte() const {
    return toInt();
}
