#ifndef WIND_CONDITIONS_H
#define WIND_CONDITIONS_H

#include <ostream>
#include "Direction.h"

class WindConditions {
private:
    Direction blowsToward_;
    int speedKmh_;
    int flightLimitKmh_;
public:
    WindConditions(Direction blowsToward, int speedKmh, int flightLimitKmh);
    Direction blowsToward() const;
    int speedKmh() const;
    bool allowsFlight() const;

    friend std::ostream& operator<<(std::ostream& os, const WindConditions& wind);
};
#endif