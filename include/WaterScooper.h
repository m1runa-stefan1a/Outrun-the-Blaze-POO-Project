#ifndef WATER_SCOOPER_H
#define WATER_SCOOPER_H
#include "Aircraft.h"

class WaterScooper : public Aircraft {
public:
    WaterScooper(std::string name, int capacity, int maxWindKmh, const HexCoord& base);
    std::unique_ptr<Aircraft> clone() const override;
private:
    void doWorkOnStation(ForestMap &map, int refillPerStep, int dropPerStep) override;
    void printDetails(std::ostream &os) const override;
};
#endif