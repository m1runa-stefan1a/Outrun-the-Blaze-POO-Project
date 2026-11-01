#include "AirOpsDispatcher.h"
#include <stdexcept>
#include <utility>

AirOpsDispatcher::AirOpsDispatcher(std::vector<AirTanker> fleet, int refillPerStep, int dropPerStep)
    : fleet_{std::move(fleet)}, dropPerStep_{dropPerStep}, refillPerStep_{refillPerStep} {
        if (fleet_.empty()) {
            throw std::invalid_argument("AirOpsDispatcher::AirOpsDispatcher: fleet can't be empty");
        }
        if (refillPerStep_ <= 0 || dropPerStep_ <= 0) {
            throw std::invalid_argument("AirOpsDispatcher:: flow rate must be greater than zero");
        }
}

void AirOpsDispatcher::dispatch(std::size_t tankerIndex, const HexCoord& target, const ForestMap& map) {
    if (!map.contains(target)) {
        throw std::invalid_argument("AirOpsDispatcher: target does not exist");
    }
    if (tankerIndex >= fleet_.size()) {
        throw std::out_of_range("AirOpsDispatcher: nu exista aeronava cu acest numar");
    }
    fleet_[tankerIndex].sendTo(target);
}
void AirOpsDispatcher::advance(const WindConditions& wind, ForestMap& map) {
    for (AirTanker tanker : fleet_) {
        tanker.advance(wind);
        if (tanker.isOnStation()) {
            workOnStation(tanker, map);
        }
    }
}
void AirOpsDispatcher::workOnStation(AirTanker& tanker, ForestMap& map) const {
    const HexCoord& where = tanker. position();
    if (map.isLake(where)) {
        tanker.refill(refillPerStep_);
    }
    else if (tanker.hasWater()&&map.isBurning(where)) {
        const int dropped=tanker.dropWater(dropPerStep_);
        map.douse(where, dropped);
    }
}
std::ostream& operator<<(std::ostream& os, const AirOpsDispatcher& dispatcher) {
    os<< "fleet("<< dispatcher.fleet_.size() << " aeronave): ";
    for (std::size_t i = 0; i < dispatcher.fleet_.size(); ++i) {
        os << "\n "<<i<<". "<<dispatcher.fleet_[i];
    }
    return os;
}