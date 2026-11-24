#ifndef AIRCRAFT_FACTORY_H
#define AIRCRAFT_FACTORY_H
#include <memory>
#include <json/json.hpp>
#include "Aircraft.h"
class AircraftFactory {
public:
    AircraftFactory() = delete;
    static std::unique_ptr<Aircraft> fromJson(const nlohmann::json& data);
};
#endif