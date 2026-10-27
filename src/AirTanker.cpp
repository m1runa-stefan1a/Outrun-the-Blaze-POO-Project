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
void  AirTanker::sendTo(const HexCoord& destination) {
    destination_ = destination;
}
void AirTanker::advance(const WindConditions& wind) {
    if (!canFlyIn(wind)) {
        if (isAtBase()) {
            state_ = State::Grounded;
            return;
        }
        destination_ = base_;
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
    if (!hasArrived()||isAtBase()) {
    throw std::logic_error(name_ + " can't " + action + ": not above the target");
    }
}
std::ostream& operator<< (std::ostream& os, const AirTanker& tanker) {
    os << tanker.name_ << " [" << tanker.state_ << "] at " << tanker.position_
    << ", tank " << tanker.payload_;
    return os;
}