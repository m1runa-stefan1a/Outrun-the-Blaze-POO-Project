#include "WindConditions.h"

#include <stdexcept>

WindConditions::WindConditions(Direction blowsToward, int speedKmh)
    : blowsToward_{blowsToward}, speedKmh_{speedKmh} {
    if (speedKmh_ < 0 ) {
        throw std::invalid_argument("WindConditions: invalid speed kmh");
    }
}

Direction WindConditions::blowsToward() const {
    return blowsToward_;
}

int WindConditions::speedKmh() const {
    return speedKmh_;
}

std::ostream& operator<<(std::ostream& os, const WindConditions& wind) {
    os << "vant " << wind.speedKmh_ << " km/h spre " << wind.blowsToward_;
    return os;
}