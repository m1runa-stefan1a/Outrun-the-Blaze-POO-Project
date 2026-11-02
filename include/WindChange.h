#ifndef WIND_CHANGE_H
#define WIND_CHANGE_H
#include <optional>
#include <ostream>
#include "Direction.h"
#include "WindConditions.h"
class WindChange {
public:
    WindChange(int step, int speedKmh, std::optional<Direction> direction);
    bool happensAt(int step) const;
    bool isAfter(int step) const;
    bool isBefore(const WindChange& other) const;
    WindConditions applyTo(const WindConditions& current) const;
    friend std::ostream& operator<<(std::ostream& os, const WindChange& change);
private:
    int step_;
    int speedKmh_;
    std::optional<Direction> direction_;
};
#endif
