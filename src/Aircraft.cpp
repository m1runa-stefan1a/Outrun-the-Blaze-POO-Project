#include "Aircraft.h"
#include <stdexcept>
#include <utility>
#include <string>

namespace {
    constexpr int windWarningPercent = 80;
    std::ostream& operator<< (std::ostream& os, Aircraft::State state) {
        switch (state) {
            case Aircraft::State::AtBase:os << "AtBase"; break;
            case Aircraft::State::Grounded:os << "Grounded"; break;
            case Aircraft::State::Flying:os << "Flying"; break;
            case Aircraft::State::Refilling:os << "Refilling"; break;
            case Aircraft::State::Dropping:os << "Dropping"; break;
            case Aircraft::State::Hovering:os << "Hovering"; break;
            case Aircraft::State::Crashed:os << "Crashed"; break;
        }
        return os;
    }
}
Aircraft::Aircraft(std::string name, int capacity, int maxWindKmh, const HexCoord& base)
    : name_{std::move(name) + "-" + std::to_string(++createdCount_)}, payload_{capacity}, maxWindKmh_{maxWindKmh}, base_{base}, position_{base}, destination_{base} {
    if (maxWindKmh_ <= 0) {
        throw std::invalid_argument{"maxWindKmh must be greater than zero"};
    }
}
void Aircraft::sendTo(const HexCoord& destination) {
    requireNotCrashed();
    destination_ = destination;
}
void Aircraft::returnToBase() {
    requireNotCrashed();
    destination_ = base_;
}

void Aircraft::advance(const WindConditions& wind) {
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
        } else {
            state_ = State::Hovering;
        }
        return;
    }
    position_ = position_.stepTowards(destination_);
    state_ = State::Flying;
}
void Aircraft::workOnStation(ForestMap& map, int refillPerStep, int dropPerStep) {
    if (isOnStation()) {
        doWorkOnStation(map, refillPerStep, dropPerStep);
    } else if (state_ == State::AtBase) {
        doWorkAtBase(refillPerStep);
    }
}
void Aircraft::doWorkAtBase(int /*refillPerStep*/) {}
bool Aircraft::canFlyIn(const WindConditions& wind) const {
    return wind.speedKmh() <= maxWindKmh_;
}
bool Aircraft::hasArrived() const {
    return position_ == destination_;
}
bool Aircraft::isOnStation() const {
    return state_ != State::Crashed && hasArrived() && !isAtBase();
}

bool Aircraft::hasWater() const {
    return !payload_.isEmpty();
}
bool Aircraft::isTankFull() const {
    return payload_.isFull();
}
const HexCoord& Aircraft::position() const {
    return position_;
}
bool Aircraft::isAirborne() const {
    return state_ != State::Crashed && !isAtBase();
}
bool Aircraft::isCrashed() const {
    return state_ == State::Crashed;
}
void Aircraft::describe(std::ostream& os) const {
    os << name_ << " [" << state_ << "]: max wind " << maxWindKmh_ << " km/h, base " << base_<< ", tank " << payload_;
    printDetails(os); 
    if (isCrashed()) {
        os << " - lost in action";
    } else if (!isAtBase()) {
        os << " - airborne, " << base_.distanceTo(position_) << " step(s) from base";
    }
}
void Aircraft::reportWindRisk(const WindConditions& wind, std::ostream& os) const {
    if (isAtBase() && !isCrashed() && !canFlyIn(wind)) {
        os << "\n[DISPATCH] " << name_ << " is grounded: wind " << wind.speedKmh() << " km/h is above its limit of " << maxWindKmh_ << " km/h";
    }
    if (isAirborne() && wind.speedKmh()*100 >= maxWindKmh_* windWarningPercent) {
        os << "\n[DISPATCH] WARNING: " << name_ << " - wind " << wind.speedKmh()<<" km/h is close to its limit of "<<maxWindKmh_<<" km/h, consider recall";
    }
}
void Aircraft::refill(int portion) {
    requireOnStation("refill");
    payload_.refill(portion);
    state_ = State::Refilling;
}
int Aircraft::dropWater(int requested) {
    requireOnStation("dropWater");
    state_ = State::Dropping;
    return payload_.release(requested);
}
void Aircraft::reloadAtBase(int portion) {
    if (state_ != State::AtBase) {
        throw std::logic_error(name_ + " can only reload at its base");
    }
    payload_.refill(portion);
}

bool Aircraft::isAtBase() const {
    return position_ == base_;
}
void Aircraft::requireOnStation(const char* action) const {
    if (!isOnStation()) {
    throw std::logic_error(name_ + " can't " + action + ": not above the target");
    }
}
void Aircraft::requireNotCrashed() const {
    if (state_ == State::Crashed) {
        throw std::logic_error(name_ + " has crashed and no longer accepts commands");
    }
}
std::ostream& operator<< (std::ostream& os, const Aircraft& aircraft) {
    os << aircraft.name_ << " [" << aircraft.state_ << "] at " << aircraft.position_;
    if (!aircraft.hasArrived() && !aircraft.isCrashed()) {
        os << " -> " << aircraft.destination_;
    }
    os << ", tank " << aircraft.payload_;
    aircraft.printDetails(os);
    return os;
}
