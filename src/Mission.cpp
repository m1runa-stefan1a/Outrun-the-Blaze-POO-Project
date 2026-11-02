#include "Mission.h"
#include <stdexcept>
#include <utility>

namespace {
    constexpr int maxLostPercent = 25;
    constexpr int lostWarningPercent = 20;
    std::ostream& operator<<(std::ostream& os, Mission::Status status) {
        switch (status) {
            case Mission::Status::InProgress: os << "In Progress"; break;
            case Mission::Status::Won: os << "Won"; break;
            case Mission::Status::Lost: os << "Lost"; break;
        }
        return os;
    }
}
Mission::Mission(std::string name, const WeatherForecast& weather, ForestMap map, AirOpsDispatcher dispatcher, const std::vector<HexCoord>& fires, int timeLimit)
    : name_{std::move(name)}, weather_{weather}, map_{std::move(map)}, dispatcher_{std::move(dispatcher)}, timeLimit_{timeLimit} {
    if (timeLimit_ <= 0) {
        throw std::invalid_argument("Mission:timeLimit must be greater than 0.");
    }
    if (fires.empty()) {
        throw std::invalid_argument("Mission:mission must have at least one fire");
    }
    for (const HexCoord& fire : fires) {
        map_.ignite(fire);
    }
}
Mission::Mission(const Mission& other)
    :name_{other.name_}, weather_{other.weather_}, map_{other.map_}, dispatcher_{other.dispatcher_}, timeLimit_{other.timeLimit_}, elapsed_ {other.elapsed_} {}
Mission& Mission::operator=(const Mission& other) {
    if (this != &other) {
        name_=other.name_;
        weather_=other.weather_;
        map_=other.map_;
        dispatcher_=other.dispatcher_;
        timeLimit_=other.timeLimit_;
        elapsed_=other.elapsed_;
    }
    return *this;
}
Mission::~Mission() = default;

void Mission::dispatch(std::size_t tankerIndex, const HexCoord &target) {
    dispatcher_.dispatch(tankerIndex, target, map_);
}

void Mission::recall(std::size_t tankerIndex) {
    dispatcher_.recall(tankerIndex);
}

void Mission::advance() {
    if (status()!= Status::InProgress) {
        throw std::logic_error("Mission: mission ended");
    }
    weather_.update(elapsed_ + 1);
    dispatcher_.advance(weather_.current(), map_);
    map_.spreadFire(weather_.current());
    ++elapsed_;
}
Mission::Status Mission::status() const {
    if (map_.isCabinLost()||map_.lostPercent()>maxLostPercent) {
        return Status::Lost;
    }
    if (!map_.hasActiveFire()||elapsed_>= timeLimit_) {
        return Status::Won;
    }
    return Status::InProgress;
}

void Mission::reportAlerts(std::ostream &os) const {
    if (status() != Status::InProgress) {
        return;
    }
    dispatcher_.reportWindRisks(weather_.current(), os);
    os << "\n[DISPATCH] fire: " <<map_.burningCount()<<" sectors burning, "<< map_.lostPercent() <<"% of the forest is lost (mission lost above "<<maxLostPercent<<"%)";
    if (map_.lostPercent() >= lostWarningPercent) {
        os<< "\n[DISPATCH] WARNING: forest loss is close to the limit!";
    }
    if (map_.isCabinThreatened()) {
        os<< "\n[DISPATCH] WARNING: fire next to the cabin!";
    }
}

std::ostream& operator << (std::ostream& os, const Mission& mission) {
    os<< "=== "<<mission.name_<<" | step " <<mission.elapsed_ <<'/'<<mission.timeLimit_<< " | "
    <<mission.weather_ <<" | " <<mission.status()<<" ===\n"<<mission.map_<<'\n'<<mission.dispatcher_;
    mission.reportAlerts(os);
    return os;
}
