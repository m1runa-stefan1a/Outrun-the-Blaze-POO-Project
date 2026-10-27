#ifndef WILDFIRE_SECTOR_H
#define WILDFIRE_SECTOR_H
#include <ostream>
#include "HexCoord.h"

class WildfireSector {
public:
    enum class Terrain{ Forest, Lake, Cabin};
    WildfireSector(const HexCoord& position, Terrain terrain);
    void ignite();
    void addHeat(int amount);
    void advanceFire();
    void douse(int liters);
    int heatOutput(int humidityPercent) const;
    bool isLost() const;
    bool isLake() const;
    bool isCabin() const;
    const HexCoord& position() const;
    friend std::ostream& operator<<(std::ostream& os, const WildfireSector& sector);
private:
    enum class State { Intact, Burning, Burned};
    bool canIgnite() const;
    HexCoord position_;
    Terrain terrain_;
    State state_ = State::Intact;
    int heat_ = 0;
    int intensity_ = 0;
};
#endif
