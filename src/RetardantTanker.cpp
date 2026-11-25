#include "RetardantTanker.h"
#include <utility>

RetardantTanker::RetardantTanker(std::string name, int capacity, int maxWindKmh, const HexCoord& base)
    : Aircraft{std::move(name), capacity, maxWindKmh, base} {}

std::unique_ptr<Aircraft> RetardantTanker::clone() const {
    return std::make_unique<RetardantTanker>(*this);
}

void RetardantTanker::doWorkOnStation(ForestMap& map, int /*refillPerStep*/, int dropPerStep) {
    if (hasWater() && map.applyRetardant(position())) {
        dropWater(dropPerStep);
        ++protectedSectors_;
    }
}
void RetardantTanker::doWorkAtBase(int refillPerStep) {
    if (!isTankFull()) {
        reloadAtBase(refillPerStep);
    }
}
void RetardantTanker::printDetails(std::ostream& os) const {
    os << ", retardant tanker (reloads only at base, " << protectedSectors_ << " sector(s) protected)";
}