#include "MissionCatalog.h"
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>

#include <json/json.hpp>
 namespace {
     Direction directionFromText(const std::string& text) {
         for (const Direction direction : allDirections) {
             std::ostringstream name;
             name << direction;
             if (name.str() == text) {
                 return direction;
             }
         }
         throw std::runtime_error("unknown wind direction: "+text);
     }
     Mission::Difficulty difficultyFromText(const std::string& text) {
         if (text == "easy") {
             return Mission::Difficulty::Easy;
         }
         if (text == "medium") {
             return Mission::Difficulty::Medium;
         }
         if (text == "hard") {
             return Mission::Difficulty::Hard;
         }
         throw std::runtime_error("unknown difficulty: "+text);
     }
     Mission parseMission(const nlohmann::json& data) {
         const ForestMap map{
             data.at("map").get<std::vector<std::string>>(),
             data.at("humidityPercent").get<int>()};
         std::vector<AirTanker> fleet;
         for (const nlohmann::json& tanker:data.at("tankers")) {
             fleet.emplace_back(tanker.at("name").get<std::string>(), tanker.at("capacity").get<int>(),
                                tanker.at("maxWindKmh").get<int>(),
                                HexCoord{tanker.at("row").get<int>(), tanker.at("col").get<int>()});
         }
         const AirOpsDispatcher dispatcher{fleet, data.at("refillPerStep").get<int>(), data.at("dropPerStep").get<int>()};
         const WindConditions wind{directionFromText(data.at("windDirection").get<std::string>()),data.at("windSpeedKmh").get<int>()};
         std::vector<WindChange> changes;
         for (const nlohmann::json& change : data.at("windChanges")) {
             std::optional<Direction> direction;
             if (change.contains("direction")) {
                 direction = directionFromText(change.at("direction").get<std::string>());
             }
             changes.emplace_back(change.at("step").get<int>(), change.at("speedKmh").get<int>(), direction);
         }
         const WeatherForecast weather{wind, changes};
         std::vector<HexCoord> fires;
         for (const nlohmann::json& fire : data.at("fires")) {
             fires.emplace_back(fire.at("row").get<int>(), fire.at("col").get<int>());
         }
         return Mission{data.at("name").get<std::string>(),
               difficultyFromText(data.at("difficulty").get<std::string>()), weather, map, dispatcher, fires, data.at("timeLimit").get<int>()};
     }
 }
MissionCatalog::MissionCatalog(const std::string& path) {
     std::ifstream file{path};
     if (!file) {
         throw std::runtime_error("cannot open file: "+path);
     }
     const nlohmann::json data = nlohmann::json::parse(file);
     for (const nlohmann::json& mission : data.at("missions")) {
         missions_.push_back(parseMission(mission));
     }
     if (missions_.empty()) {
         throw std::runtime_error("MissionCatalog: no missions in "+path);
     }
 }

std::size_t MissionCatalog::size() const {
     return missions_.size();
 }

const Mission& MissionCatalog::at(std::size_t index) const {
     if (index >= missions_.size()) {
         throw std::out_of_range("MissionCatalog: no mission with this number");
     }
     return missions_[index];
 }
std::ostream& operator<<(std::ostream& os, const MissionCatalog& catalog) {
     os << "=== MISSIONS ===";
     for (std::size_t i = 0; i < catalog.missions_.size(); ++i) {
         os << '\n' << i + 1 << ". ";
         catalog.missions_[i].describe(os);
     }
     return os;
 }