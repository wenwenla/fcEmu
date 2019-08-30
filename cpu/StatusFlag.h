//
// Created by DELL on 2019/8/30.
//

#ifndef FCEMU_STATUSFLAG_H
#define FCEMU_STATUSFLAG_H

#include <bitset>
#include "../utility.h"

class StatusFlag {
public:
    StatusFlag();
    void reset();

    void setNegative(bool flag);
    void setOverflow(bool flag);
    void setBreak(bool flag);
    void setDecimal(bool flag);
    void setIntDisable(bool flag);
    void setZero(bool flag);
    void setCarry(bool flag);

    bool getNegative() const;
    bool getOverflow() const;
    bool getBreak() const;
    bool getDecimal() const;
    bool getIntDisable() const;
    bool getZero() const;
    bool getCarry() const;

    int toInt() const;

    Byte toByte() const;
private:
    std::bitset<8> _flag;
};


#endif //FCEMU_STATUSFLAG_H
