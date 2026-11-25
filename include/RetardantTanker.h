#ifndef RETARDANT_TANKER_H
#define RETARDANT_TANKER_H
#include "Aircraft.h"

class RetardantTanker: public Aircraft {
public:
    RetardantTanker(std::string name, int capacity, int maxWindKmh, const HexCoord& base);
    std::unique_ptr<Aircraft> clone() const override;
private:
    void doWorkOnStation(ForestMap& map, int refillPerStep, int dropPerStep) override;
    void doWorkAtBase(int refillPerStep) override;
    void printDetails(std::ostream &os) const override;
    int protectedSectors_ = 0;
};
#endif