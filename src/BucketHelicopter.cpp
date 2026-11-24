#include "BucketHelicopter.h"
#include <utility>
namespace {
    constexpr int wetSteps = 3;
}
BucketHelicopter::BucketHelicopter(std::string name, int capacity, int maxWindKmh, const HexCoord& base)
    : Aircraft{std::move(name), capacity, maxWindKmh, base} {}

std::unique_ptr<Aircraft> BucketHelicopter::clone() const {
    return std::make_unique<BucketHelicopter>(*this);
}

void BucketHelicopter::doWorkOnStation(ForestMap &map, int refillPerStep, int dropPerStep) {
    const HexCoord& where = position();
    if (map.isLake(where)) {
        if (!isTankFull()) {
            refill(refillPerStep);
        }
    }else if (hasWater() && map.isBurning(where)) {
        const int dropped = dropWater(dropPerStep);
        map.douse(where, dropped);
        map.soak(where, wetSteps);
    }
}
void BucketHelicopter::printDetails(std::ostream& os) const {
    os << ", bucket helicopter (soaks the sector for " << wetSteps << " steps)";
}