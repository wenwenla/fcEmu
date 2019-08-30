//
// Created by DELL on 2019/8/30.
//

#ifndef FCEMU_STATUSFLAG_H
#define FCEMU_STATUSFLAG_H

#include <bitset>

class StatusFlag {
public:
    StatusFlag();
    void reset();

    void setNegative(int flag);
    void setOverflow(int flag);
    void setBreak(int flag);
    void setDecimal(int flag);
    void setIntDisable(int flag);
    void setZero(int flag);
    void setCarry(int flag);

    int getNegative() const;
    int getOverflow() const;
    int getBreak() const;
    int getDecimal() const;
    int getIntDisable() const;
    int getZero() const;
    int getCarry() const;
private:
    std::bitset<8> _flag;
};


#endif //FCEMU_STATUSFLAG_H
