#include "WindChange.h"
#include<stdexcept>

WindChange::WindChange(int step, int speedKmh, std::optional<Direction> direction)
    : step_{step}, speedKmh_{speedKmh}, direction_{direction} {
    if (step_ <= 0) {
        throw std::invalid_argument{"step must be greater than zero"};
    }
    if (speedKmh_ < 0) {
        throw std::invalid_argument{"wind speed can't be negative"};
    }
}

bool WindChange::happensAt(int step) const {
    return step_ == step;
}
bool WindChange::isAfter(int step) const {
    return step_ > step;
}
bool WindChange::isBefore(const WindChange& other) const {
    return step_ < other.step_;
}

WindConditions WindChange::applyTo(const WindConditions& current) const {
    return WindConditions{direction_.value_or(current.blowsToward()), speedKmh_};
}
std::ostream& operator<<(std::ostream& os, const WindChange& change) {
    os << change.speedKmh_ << "km/h";
    if (change.direction_.has_value()) {
        os << " towards "<< *change.direction_;
    }
    os<< " at step "<<change.step_;
    return os;
}