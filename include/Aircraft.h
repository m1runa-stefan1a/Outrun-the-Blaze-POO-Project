#ifndef AIRCRAFT_H
#define AIRCRAFT_H
#include <memory>
#include <ostream>
#include <string>

#include "ForestMap.h"
#include "HexCoord.h"
#include "WaterPayload.h"
#include "WindConditions.h"

class Aircraft {
public:
    enum class State{AtBase, Grounded, Flying, Refilling, Dropping, Hovering, Crashed};
    Aircraft(std::string name, int capacity, int maxWindKmh, const HexCoord& base);
    virtual ~Aircraft()= default;
    virtual std::unique_ptr<Aircraft> clone() const = 0;

    void sendTo(const HexCoord& destination);
    void returnToBase();
    void advance(const WindConditions& wind);
    void workOnStation(ForestMap& map, int refillPerStep, int dropPerStep);
    bool isCrashed() const;
    void describe(std::ostream& os) const;
    void reportWindRisk( const WindConditions& wind, std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const Aircraft& aircraft);
protected:
    Aircraft(const Aircraft& other)=default;
    Aircraft& operator=(const Aircraft& other)=default;
    const HexCoord& position() const;
    bool hasWater() const;
    bool isTankFull() const;
    void refill(int portion);
    int dropWater(int requested);
    void reloadAtBase(int portion);
private:
    virtual void doWorkOnStation(ForestMap& map, int refillPerStep, int dropPerStep)=0;
    virtual void printDetails(std::ostream& os) const =0;
    virtual void doWorkAtBase(int refillPerStep);
    bool canFlyIn(const WindConditions& wind) const;
    bool hasArrived() const;
    bool isOnStation() const;
    bool isAirborne() const;
    bool isAtBase() const;
    void requireNotCrashed() const;
    void requireOnStation(const char* action) const;
    inline static int createdCount_ = 0;
    std::string name_;
    WaterPayload payload_;
    int maxWindKmh_;
    HexCoord base_;
    HexCoord position_;
    HexCoord destination_;
    State state_ =State::AtBase;
};
#endif