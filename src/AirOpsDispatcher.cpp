#include "AirOpsDispatcher.h"
#include "RetardantTanker.h"
#include "GameErrors.h"
#include <utility>
#include <algorithm>

AirOpsDispatcher::AirOpsDispatcher(std::vector<std::unique_ptr<Aircraft>> fleet, int refillPerStep, int dropPerStep)
        : fleet_{std::move(fleet)}, refillPerStep_{refillPerStep}, dropPerStep_{dropPerStep} {
        if (fleet_.empty()) {
            throw std::invalid_argument("AirOpsDispatcher::AirOpsDispatcher: fleet can't be empty");
        }
        if (refillPerStep_ <= 0 || dropPerStep_ <= 0) {
            throw std::invalid_argument("AirOpsDispatcher:: flow rate must be greater than zero");
        }
}
AirOpsDispatcher::AirOpsDispatcher(const AirOpsDispatcher& other)
    : refillPerStep_{other.refillPerStep_}, dropPerStep_{other.dropPerStep_} {
    fleet_.reserve(other.fleet_.size());
    for (const std::unique_ptr<Aircraft>& aircraft : other.fleet_) {
        fleet_.push_back(aircraft->clone());
    }
}

AirOpsDispatcher& AirOpsDispatcher::operator=(AirOpsDispatcher other) {
    swap(*this, other);
    return *this;
}

void swap(AirOpsDispatcher& first, AirOpsDispatcher& second) noexcept {
    using std::swap;
    swap(first.fleet_, second.fleet_);
    swap(first.refillPerStep_, second.refillPerStep_);
    swap(first.dropPerStep_, second.dropPerStep_);
}
void AirOpsDispatcher::dispatch(std::size_t aircraftIndex, const HexCoord& target, const ForestMap& map, std::ostream& os) {
    if (!map.contains(target)) {
        throw CommandError{"this sector is not on the map"};
    }
    Aircraft& aircraft = aircraftAt(aircraftIndex);
    aircraft.sendTo(target);
    if (dynamic_cast<const RetardantTanker*>(&aircraft) != nullptr && map.isBurning(target)) {
        os << "[DISPATCH] WARNING: retardant does not put out fires - drop it on the forest in front of the fire\n";
    }
}

void AirOpsDispatcher::recall(std::size_t aircraftIndex) {
    aircraftAt(aircraftIndex).returnToBase();
}

void AirOpsDispatcher::advance(const WindConditions& wind, ForestMap& map) {
    for (const std::unique_ptr<Aircraft>& aircraft : fleet_) {
        aircraft->advance(wind);
        aircraft->workOnStation(map, refillPerStep_, dropPerStep_);
    }
}
std::size_t AirOpsDispatcher::fleetSize() const {
    return fleet_.size();
}
int AirOpsDispatcher::crashedCount() const {
    return static_cast<int>(std::count_if(fleet_.begin(), fleet_.end(),
        [](const std::unique_ptr<Aircraft>& aircraft) { return aircraft->isCrashed(); }));
}
void AirOpsDispatcher::describeFleet(std::ostream& os) const {
    os << "=== FLEET STATUS ===";
    for (std::size_t i = 0; i < fleet_.size(); ++i) {
        os << "\n " << i << ". ";
        fleet_[i]->describe(os);
    }
}
void AirOpsDispatcher::reportWindRisks(const WindConditions& wind, std::ostream& os) const {
    for (const std::unique_ptr<Aircraft>& aircraft : fleet_) {
        aircraft->reportWindRisk(wind, os);
    }
}
Aircraft& AirOpsDispatcher::aircraftAt(std::size_t aircraftIndex) {
    if (aircraftIndex >= fleet_.size()) {
        throw CommandError{"there is no aircraft number " + std::to_string(aircraftIndex)};
    }
    return *fleet_[aircraftIndex];
}
std::ostream& operator<<(std::ostream& os, const AirOpsDispatcher& dispatcher) {
    os << "fleet(" << dispatcher.fleet_.size() << " aircraft): ";
    for (std::size_t i = 0; i < dispatcher.fleet_.size(); ++i) {
        os << "\n " << i << ". " << *dispatcher.fleet_[i];
    }
    return os;

}