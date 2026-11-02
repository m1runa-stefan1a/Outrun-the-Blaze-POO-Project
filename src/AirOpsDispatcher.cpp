#include "AirOpsDispatcher.h"
#include <stdexcept>
#include <utility>
#include <algorithm>

AirOpsDispatcher::AirOpsDispatcher(std::vector<AirTanker> fleet, int refillPerStep, int dropPerStep)
        : fleet_{std::move(fleet)}, refillPerStep_{refillPerStep}, dropPerStep_{dropPerStep} {
        if (fleet_.empty()) {
            throw std::invalid_argument("AirOpsDispatcher::AirOpsDispatcher: fleet can't be empty");
        }
        if (refillPerStep_ <= 0 || dropPerStep_ <= 0) {
            throw std::invalid_argument("AirOpsDispatcher:: flow rate must be greater than zero");
        }
}

void AirOpsDispatcher::dispatch(std::size_t tankerIndex, const HexCoord& target, const ForestMap& map) {
    if (!map.contains(target)) {
        throw std::out_of_range("AirOpsDispatcher: target does not exist");    }
    if (tankerIndex >= fleet_.size()) {
        throw std::out_of_range("AirOpsDispatcher: no aircraft with this number");
    }
    fleet_[tankerIndex].sendTo(target);
}

void AirOpsDispatcher::recall(std::size_t tankerIndex) {
    if (tankerIndex >= fleet_.size()) {
        throw std::out_of_range("AirOpsDispatcher: no aircraft with this number");
    }
    fleet_[tankerIndex].returnToBase();
}

void AirOpsDispatcher::advance(const WindConditions& wind, ForestMap& map) {
    for (AirTanker& tanker : fleet_) {
        tanker.advance(wind);
        if (tanker.isOnStation()) {
            workOnStation(tanker, map);
        }
    }
}
std::size_t AirOpsDispatcher::fleetSize() const {
    return fleet_.size();
}
int AirOpsDispatcher::crashedCount() const {
    return static_cast<int>(std::count_if(fleet_.begin(), fleet_.end(),
        [](const AirTanker& tanker) { return tanker.isCrashed(); }));
}
void AirOpsDispatcher::describeFleet(std::ostream& os) const {
    os << "=== FLEET STATUS ===";
    for (std::size_t i = 0; i < fleet_.size(); ++i) {
        os << "\n " << i << ". ";
        fleet_[i].describe(os);
    }
}
void AirOpsDispatcher::reportWindRisks(const WindConditions& wind, std::ostream& os) const {
    for (const AirTanker& tanker : fleet_) {
        tanker.reportWindRisk(wind, os);
    }
}
void AirOpsDispatcher::workOnStation(AirTanker& tanker, ForestMap& map) const {
    const HexCoord& where = tanker. position();
    if (map.isLake(where)) {
        if (!tanker.isTankFull()) {
            tanker.refill(refillPerStep_);
        }
    }
    else if (tanker.hasWater()&&map.isBurning(where)) {
        const int dropped=tanker.dropWater(dropPerStep_);
        map.douse(where, dropped);
    }
}
std::ostream& operator<<(std::ostream& os, const AirOpsDispatcher& dispatcher) {
    os<< "fleet("<< dispatcher.fleet_.size() << " aircraft): ";
    for (std::size_t i = 0; i < dispatcher.fleet_.size(); ++i) {
        os << "\n "<<i<<". "<<dispatcher.fleet_[i];
    }
    return os;
}