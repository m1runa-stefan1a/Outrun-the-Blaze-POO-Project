#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <optional>

#include "Mission.h"

#include <json/json.hpp>

namespace {
    enum class Input { Available, Ended };
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
    Mission loadMission(const std::string& path) {
        std::ifstream file{path};
        if (!file) {
            throw std::runtime_error("cannot open file: "+path);
        }
        const nlohmann::json data= nlohmann::json::parse(file);
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
        return Mission{data.at("name").get<std::string>(), weather, map, dispatcher, fires, data.at("timeLimit").get<int>()};
    }


void skipLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
Input readDispatch(Mission& mission) {
    std::size_t tanker =0;
    int row=0;
    int col=0;
    if (!(std::cin >> tanker >> row >> col)) {
        if (std::cin.eof()) {
            return Input::Ended;
        }
        std::cin.clear();
        skipLine();
        std::cout <<"Format: send <palne> <row> <col>\n";
        return Input::Available;
    }
    const HexCoord target{row, col};
    try {
        mission.dispatch(tanker, target);
        std::cout<<"Aircraft " << tanker << " heading to " << target << '\n';
    } catch (const std::logic_error& error) {
        std::cout << "Invalid command: " << error.what() << '\n';
    }
    return Input::Available;

}
Input readRecall(Mission& mission) {
        std::size_t tanker =0;
        if (!(std::cin>>tanker)) {
            if (std::cin.eof()) {
                return Input::Ended;
            }
            std::cin.clear();
            skipLine();
            std::cout<<"Format: recall <airplane>\n";
            return Input::Available;
        }
        try {
            mission.recall(tanker);
            std::cout<<"Airplane"<<tanker<<"is returning to the base\n";
        } catch (const std::logic_error& error) {
            std::cout << "Invalid command: " << error.what() << '\n';
        }
        return Input::Available;
    }
Input readWait(Mission& mission) {
    int steps = 0;
    if (!(std::cin >> steps)) {
        if (std::cin.eof()) {
            return Input::Ended;
        }
        std::cin.clear();
        skipLine();
        std::cout<< "Format: wait <steps>\n";
        return Input::Available;
    }
    if (steps < 1) {
        std::cout<<"the number of steps must be greater than 0.\n";
        return Input::Available;
    }
    for (int i=0; i<steps && mission.status()==Mission::Status::InProgress; ++i) {
        mission.advance();
    }
    std::cout<<mission<<'\n';
    return Input::Available;
}
Input play(Mission& mission) {
    std::cout<<mission<<'\n';
    while (mission.status()==Mission::Status::InProgress) {
        std::cout<<"command: send <palne> <row> <col> | wait <steps> | recall <plane> :\n";
        std::string command;
        if (!(std::cin >> command)) {
            return Input::Ended;
        }
        Input input=Input::Available;
        if (command == "send") {
            input=readDispatch(mission);
        } else if (command == "wait") {
            input=readWait(mission);
        } else if (command == "recall") {
            input=readRecall(mission);
        }else {
            skipLine();
            std::cout<<"unknown command\n"<<command<<'\n';
        }
        if (input == Input::Ended) {
            return Input::Ended;
        }
    }
    return Input::Available;
}
bool askRetry() {
    std::string answer;
    while (true) {
        std::cout<<"Do you want to retry? (y/n) ";
        if (!(std::cin >> answer)) {
            return false;
        }
        if (answer == "y") {
            return true;
        }
        if (answer == "n") {
            return false;
        }
        std::cout<<"respond with y/n";
    }
}
}
int main() {
    try {
        const Mission initial = loadMission("assets/missions.json");
        Mission attempt{initial};
        while (play(attempt) == Input::Available && askRetry()) {
            attempt = initial;
        }
    } catch (const std::exception& error) {
        std::cout << "Cannot load the mission: " << error.what() << '\n';
        return 1;
    }
    std::cout<<"End.\n";
    return 0;
}