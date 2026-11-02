#include "WeatherForecast.h"
#include <utility>

WeatherForecast::WeatherForecast(const WindConditions& initial, std::vector<WindChange> changes)
    : current_{initial}, changes_{std::move(changes)} {}

void WeatherForecast::update(int step) {
    step_ = step;
    for (const WindChange& change: changes_) {
        if (change.happensAt(step)) {
            current_ = change.applyTo(current_);
        }
    }
}
const WindConditions& WeatherForecast::current() const {
    return current_;
}

const WindChange* WeatherForecast::nextChange() const {
    const WindChange* next = nullptr;
    for (const WindChange& change: changes_) {
        if (change.isAfter(step_) &&(next==nullptr || change.isBefore(*next))) {
            next = &change;
        }
    }
    return next;
}
std::ostream& operator<<(std::ostream& os, const WeatherForecast& forecast) {
    os<<forecast.current_;
    const WindChange* next = forecast.nextChange();
    if (next!=nullptr) {
        os<<" (forecast: "<<*next<<")";
    }
    return os;
}