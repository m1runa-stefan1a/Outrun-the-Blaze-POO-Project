#include "WindConditions.h"

#include <stdexcept>

WindConditions::WindConditions(Direction blowsToward, int speedKmh, int flightLimitKmh)
    : blowsToward_{blowsToward}, speedKmh_{speedKmh}, flightLimitKmh_{flightLimitKmh} {
    if (speedKmh_ < 0 || flightLimitKmh_ <= 0) {
        throw std::invalid_argument("WindConditions: viteza negativa sau limita de zbor invalida");
    }
}

Direction WindConditions::blowsToward() const {
    return blowsToward_;
}

int WindConditions::speedKmh() const {
    return speedKmh_;
}

bool WindConditions::allowsFlight() const {
    return speedKmh_ <= flightLimitKmh_;
}

std::ostream& operator<<(std::ostream& os, const WindConditions& wind) {
    os << "vant " << wind.speedKmh_ << " km/h spre " << wind.blowsToward_;
    if (!wind.allowsFlight()) {
        os << " (zboruri suspendate)";
    }
    return os;
}