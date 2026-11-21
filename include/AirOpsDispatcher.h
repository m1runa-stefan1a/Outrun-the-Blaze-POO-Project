#ifndef AIR_OPS_DISPATCHER_H
#define AIR_OPS_DISPATCHER_H
#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

#include "Aircraft.h"
#include "ForestMap.h"
#include "HexCoord.h"
#include "WindConditions.h"

class AirOpsDispatcher {
public:
    AirOpsDispatcher(std::vector<std::unique_ptr<Aircraft>> fleet, int refillPerStep, int dropPerStep);
    AirOpsDispatcher(const AirOpsDispatcher& other);
    AirOpsDispatcher(AirOpsDispatcher&& other) noexcept = default;
    AirOpsDispatcher& operator=(AirOpsDispatcher other);
    ~AirOpsDispatcher() = default;
    friend void swap(AirOpsDispatcher& first, AirOpsDispatcher& second) noexcept;
    void dispatch(std::size_t aircraftIndex, const HexCoord& target, const ForestMap& map);
    void recall(std::size_t aircraftIndex);
    void advance(const WindConditions& wind, ForestMap& map);
    int crashedCount() const;
    std::size_t fleetSize() const;
    void describeFleet(std::ostream& os) const;
    void reportWindRisks(const WindConditions& wind, std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const AirOpsDispatcher& dispatcher);
private:
    Aircraft& aircraftAt(std::size_t aircraftIndex);
    std::vector<std::unique_ptr<Aircraft>> fleet_;
    int refillPerStep_;
    int dropPerStep_;
};
#endif