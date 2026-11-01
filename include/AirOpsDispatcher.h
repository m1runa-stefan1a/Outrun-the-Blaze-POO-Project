#ifndef AIR_OPS_DISPATCHER_H
#define AIR_OPS_DISPATCHER_H
#include <cstddef>
#include <ostream>
#include <vector>

#include "AirTanker.h"
#include "ForestMap.h"
#include "HexCoord.h"
#include "WindConditions.h"

class AirOpsDispatcher {
public:
    AirOpsDispatcher(std::vector<AirTanker> fleet, int refillPerStep, int dropPerStep);
    void dispatch(std::size_t tankerIndex, const HexCoord& target, const ForestMap& map);
    void recall(std::size_t tankerIndex);
    void advance(const WindConditions& wind, ForestMap& map);
    friend std::ostream& operator<<(std::ostream& os, const AirOpsDispatcher& dispatcher);
private:
    void workOnStation(AirTanker& tanker, ForestMap& map) const;
    std::vector<AirTanker> fleet_;
    int refillPerStep_;
    int dropPerStep_;
};
#endif