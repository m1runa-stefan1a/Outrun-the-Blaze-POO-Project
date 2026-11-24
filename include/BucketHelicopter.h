#ifndef BUCKET_HELICOPTER_H
#define BUCKET_HELICOPTER_H
#include "Aircraft.h"

class BucketHelicopter:public Aircraft {
public:
    BucketHelicopter(std::string name, int capacity, int maxWindKmh, const HexCoord& base);
    std::unique_ptr<Aircraft> clone() const override;
private:
    void doWorkOnStation(ForestMap& map, int refillPerStep, int dropPerStep) override;
    void printDetails(std::ostream &os) const override;
};
#endif