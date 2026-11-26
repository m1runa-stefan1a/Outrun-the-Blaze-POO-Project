#include "WaterPayload.h"
#include "GameErrors.h"
#include <algorithm>
#include <stdexcept>

WaterPayload::WaterPayload(int capacity) : capacity_(capacity) {
    if (capacity_ <= 0) {
        throw InvalidSetupError{"tank capacity must be greater than zero"};
    }
}

void WaterPayload::refill(int portion) {
    if (portion < 0) {
        throw SimulationError{"refill portion can't be negative"};
    }
    current_ = std::min(capacity_, current_ + portion);
}

int WaterPayload::release(int requested) {
    if (requested < 0) {
        throw SimulationError{"released quantity can't be negative"};
    }
    const int released = std::min(requested, current_);
    current_ -= released;
    return released;
}

bool WaterPayload::isEmpty() const {
    return current_ == 0;
}

bool WaterPayload::isFull() const {
    return current_ == capacity_;
}

int WaterPayload::fillPercent() const {
    return current_ * 100 / capacity_;
}

std::ostream &operator<<(std::ostream &os, const WaterPayload &payload) {
    os << payload.current_ << '/' << payload.capacity_<< "L ("<<payload.fillPercent()<<"%)";
    return os;
}