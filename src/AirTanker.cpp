#include "AirTanker.h"
#include <stdexcept>
#include <utility>

namespace {
    std::ostream& operator<< (std::ostream& os, AirTanker::State state) {
        switch (state) {
            case AirTanker::State::AtBase:os << "AtBase"; break;
            case AirTanker::State::Grounded:os << "Grounded"; break;
            case AirTanker::State::Flying:os << "Flying"; break;
            case AirTanker::State::Refilling:os << "Refilling"; break;
            case AirTanker::State::Dropping:os << "Dropping"; break;
            case AirTanker::State::Crashed:os << "Crashed"; break;
        }
        return os;
    }
}
AirTanker::AirTanker(std::string name, int capacity, int maxWindKmh, const HexCoord& base)
    : name_{std::move(name)}, payload_{capacity}, maxWindKmh_{maxWindKmh}, base_{base}, position_{base}, destination_{base} {
    if (maxWindKmh_ <= 0) {
        throw std::invalid_argument{"maxWindKmh must be greater than zero"};
    }
}
void AirTanker::sendTo(const HexCoord& destination) {
    requireNotCrashed();
    destination_ = destination;
}
void AirTanker::returnToBase() {
    requireNotCrashed();
    destination_ = base_;
}

void AirTanker::advance(const WindConditions& wind) {
    if (state_ == State::Crashed) {
        return;
    }
    if (!canFlyIn(wind)) {
        if (isAtBase()) {
            state_ = State::Grounded;
            return;
        }else {
            state_ =State::Crashed;
        }
        return;
    }
    if (hasArrived()) {
        if (isAtBase()) {
            state_ = State::AtBase;
        }
        return;
    }
    position_ = position_.stepTowards(destination_);
    state_ = State::Flying;
}
bool AirTanker::canFlyIn(const WindConditions& wind) const {
    return wind.speedKmh() <= maxWindKmh_;
}
bool AirTanker::hasArrived() const {
    return position_ == destination_;
}
bool AirTanker::isOnStation() const {
    return state_ != State::Crashed && hasArrived() && !isAtBase();
}

bool AirTanker::hasWater() const {
    return !payload_.isEmpty();
}
bool AirTanker::isTankFull() const {
    return payload_.isFull();
}
const HexCoord& AirTanker::position() const {
    return position_;
}
void AirTanker::refill(int portion) {
    requireOnStation("refill");
    payload_.refill(portion);
    state_ = State::Refilling;
}
int AirTanker::dropWater(int requested) {
    requireOnStation("dropWater");
    state_ = State::Dropping;
    return payload_.release(requested);
}
bool AirTanker::isAtBase() const {
    return position_ == base_;
}
void AirTanker::requireOnStation(const char* action) const {
    if (!isOnStation()) {
    throw std::logic_error(name_ + " can't " + action + ": not above the target");
    }
}
void AirTanker::requireNotCrashed() const {
    if (state_ == State::Crashed) {
        throw std::logic_error(name_ + "  has crashed and no longer accepts commands");
    }
}
std::ostream& operator<< (std::ostream& os, const AirTanker& tanker) {
    os << tanker.name_ << " [" << tanker.state_ << "] at " << tanker.position_
    << ", tank " << tanker.payload_;
    return os;
}