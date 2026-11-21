#include "WaterScooper.h"
#include <utility>

WaterScooper::WaterScooper(std::string name, int capacity, int maxWindKmh, const HexCoord& base)
    : Aircraft{std::move(name), capacity, maxWindKmh, base} {}
std::unique_ptr<Aircraft> WaterScooper::clone() const {
    return std::make_unique<WaterScooper>(*this);
}

void WaterScooper::doWorkOnStation(ForestMap &map, int refillPerStep, int dropPerStep) {
    const HexCoord& where = position();
    if (map.isLake(where)) {
        if (!isTankFull()) {
            refill(refillPerStep);
        }
    } else if (hasWater() && map.isBurning(where)) {
        const int dropped = dropWater(dropPerStep);
        map.douse(where, dropped);
    }
}

void WaterScooper::printDetails(std::ostream &os) const {
    os << ", amphibious scooper (refills on lakes)";
}
