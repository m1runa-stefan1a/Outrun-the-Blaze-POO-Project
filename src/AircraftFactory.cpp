#include "AircraftFactory.h"
#include "GameErrors.h"
#include <string>
#include <utility>

#include "BucketHelicopter.h"
#include "RetardantTanker.h"
#include "WaterScooper.h"

std::unique_ptr<Aircraft> AircraftFactory::fromJson(const nlohmann::json &data) {
    const std::string type = data.at("type").get<std::string>();
    std::string name = data.at("name").get<std::string>();
    const int capacity = data.at("capacity").get<int>();
    const int maxWindKmh = data.at("maxWindKmh").get<int>();
    const HexCoord base{data.at("row").get<int>(), data.at("col").get<int>()};
    if (type == "scooper") {
        return std::make_unique<WaterScooper>(std::move(name), capacity, maxWindKmh, base);
    }
    if (type == "helicopter") {
        return std::make_unique<BucketHelicopter>(std::move(name), capacity, maxWindKmh, base);
    }
    if (type == "retardant") {
        return std::make_unique<RetardantTanker>(std::move(name), capacity, maxWindKmh, base);
    }
    throw DataFileError{"unknown aircraft type '" + type + "'"};
}
