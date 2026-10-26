#ifndef WIND_CONDITIONS_H
#define WIND_CONDITIONS_H

#include <ostream>
#include "Direction.h"

class WindConditions {
private:
    Direction blowsToward_;
    int speedKmh_;
public:
    WindConditions(Direction blowsToward, int speedKmh);
    Direction blowsToward() const;
    int speedKmh() const;

    friend std::ostream& operator<<(std::ostream& os, const WindConditions& wind);
};
#endif