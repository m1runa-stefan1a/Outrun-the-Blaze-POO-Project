#ifndef FOREST_MAP_H
#define FOREST_MAP_H
#include <cstddef>
#include <ostream>
#include <vector>
#include <string>

#include "HexCoord.h"
#include "WildfireSector.h"
#include "WindConditions.h"

class ForestMap {
public:
    ForestMap(const std::vector<std::string>& layout, int HumidityPercent);
    void ignite(const HexCoord& where);
    bool contains(const HexCoord& where) const;
    void spreadFire(const WindConditions& wind);
    void douse(const HexCoord& where, int liters);
    bool isLake(const HexCoord& where) const;
    bool isBurning(const HexCoord& where) const;
    bool hasActiveFire() const;
    int lostPercent() const;
    bool isCabinLost() const;
    int burningCount() const;
    bool isCabinThreatened() const;
    friend std::ostream& operator<<(std::ostream& os, const ForestMap& map);
private:
    std::size_t indexOf(const HexCoord& where) const;
    int rowCount_;
    int colCount_;
    int humidityPercent_;
    std::vector<WildfireSector> sectors_;
};
#endif