#ifndef AIR_TANKER_H
#define AIR_TANKER_H
#include <ostream>
#include <string>

#include "WaterPayload.h"
#include "HexCoord.h"
#include "WindConditions.h"

class AirTanker {
public:
    enum class State{ AtBase, Grounded, Flying, Refilling, Dropping};
    AirTanker(std::string name, int capacity, int maxWindKmh, const HexCoord& base);
    void sendTo(const HexCoord& destination);
    void advance(const WindConditions& wind);
    bool canFlyIn(const WindConditions& wind) const;
    bool hasArrived() const;
    const HexCoord& position() const;
    void refill(int portion);
    int dropWater(int requested);

    friend std::ostream& operator<<(std::ostream& os, const AirTanker& tanker);
private:
    bool isAtBase() const;
    void requireOnStation(const char* action) const;
    std::string name_;
    WaterPayload payload_;
    int maxWindKmh_;
    HexCoord base_;
    HexCoord position_;
    HexCoord destination_;
    State state_ = State::AtBase;
};
#endif