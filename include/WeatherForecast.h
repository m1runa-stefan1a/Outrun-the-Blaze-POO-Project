#ifndef WEATHER_FORECAST_H
#define WEATHER_FORECAST_H

#include <ostream>
#include <vector>

#include "WindChange.h"
#include "WindConditions.h"

class WeatherForecast {
public:
    WeatherForecast(const WindConditions& initial, std::vector<WindChange> changes);
    void update(int step);
    const WindConditions& current() const;
    friend std::ostream& operator<<(std::ostream& os, const WeatherForecast& forecast);
private:
    const WindChange* nextChange() const;
    WindConditions current_;
    std::vector<WindChange> changes_;
    int step_=0;
};
#endif