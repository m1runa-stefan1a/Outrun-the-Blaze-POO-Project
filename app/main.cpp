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
#include "MissionCatalog.h"
#include "ScoreBoard.h"

#include <json/json.hpp>

namespace {
    enum class Input { Available, Ended };

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
        std::cout <<"Format: send <plane> <row> <col>\n";
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
            std::cout<<"Format: recall <plane>\n";
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
    return Input::Available;
}
void printHelp() {
    std::cout << "\n=== DISPATCHER COMMANDS ===\n"
              << "send <plane> <row> <col>  send an aircraft to a sector: over a lake it refills,\n"
              << "                          over a burning sector it drops water\n"
              << "recall <plane>            bring an aircraft back to its base (the only safe place)\n"
              << "wait <steps>              let time pass; aircraft move one sector per step\n"
              << "status                    show wind limits, bases and tanks of the whole fleet\n"
              << "help                      show this help\n"
              << "\n=== MAP SYMBOLS ===\n"
              << "P forest   ~ lake   C cabin   * burning   # burned\n"
              << "row numbers are on the left, column numbers on top\n"
              << "\n=== AIRCRAFT STATES ===\n"
              << "AtBase     safe at its base\n"
              << "Grounded   at its base, the wind is too strong to take off\n"
              << "Flying     on the way to its destination\n"
              << "Refilling  taking water from a lake\n"
              << "Dropping   dropping water on a fire\n"
              << "Hovering   waiting above its destination\n"
              << "Crashed    lost: the wind went over its limit while it was in the air\n"
              << "\n=== RULES ===\n"
              << "the mission is won when no fire is left or the time limit is reached\n"
              << "the mission is lost if the cabin burns or too much of the forest is lost\n"
              << "an aircraft in the air crashes if the wind goes over its limit - recall it in time\n"
              << "score: 1000, -20 per burned sector, -150 per crashed aircraft, +10 per remaining step\n";
}
Input play(Mission& mission, ScoreBoard& scores) {
    while (mission.status()==Mission::Status::InProgress) {
        std::cout<<mission<<'\n';
                std::cout<<"command: send <plane> <row> <col> | wait <steps> | recall <plane> | status | help :\n";
        std::string command;
        if (!(std::cin >> command)) {
            return Input::Ended;
        }
        Input input=Input::Available;
        if (command == "send") {
            input=readDispatch(mission);
        } else if (command == "wait") {
            input=readWait(mission);
        } else if (command == "status") {
            mission.describeStatus(std::cout);
            std::cout << '\n';
        } else if (command == "help") {
            printHelp();
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
    std::cout << mission << '\n';
    const MissionReport report = mission.report();
    std::cout<<report<<'\n';
    const int previousBest = scores.bestFor(report.missionName());
    if (scores.submit(report)) {
        std::cout << "NEW RECORD! (previous best: " << previousBest << ")\n";
    } else {
        std::cout << "best score for this mission: " << previousBest << '\n';
    }
    return Input::Available;
}

std::optional<std::size_t> chooseMission(const MissionCatalog& catalog, const ScoreBoard& scores) {
    while (true) {
        std::cout << "\n=== MISSIONS ===\n";
        for (std::size_t i = 0; i < catalog.size(); ++i) {
            const Mission& mission = catalog.at(i);
            std::cout << i + 1 << ". ";
            mission.describe(std::cout);
            std::cout << " | best score: " << scores.bestFor(mission.name()) << '\n';
        }
        std::cout << "choose a mission (1-" << catalog.size() << ", 0 to quit): ";
        std::size_t choice = 0;
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                return std::nullopt;
            }
            std::cin.clear();
            skipLine();
            std::cout << "please enter a number\n";
            continue;
        }
        if (choice == 0) {
            return std::nullopt;
        }
        if (choice > catalog.size()) {
            std::cout << "no mission with this number\n";
            continue;
        }
        return choice - 1;
    }
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
        std::cout<<"respond with y/n\n";
    }
}
}
int main() {
    try {
        const MissionCatalog catalog{"assets/missions.json"};
        ScoreBoard scores{"scores.json"};
        while (true) {
            const std::optional<std::size_t> choice = chooseMission(catalog, scores);
            if (!choice.has_value()) {
                break;
            }
            const Mission& initial = catalog.at(*choice);
            Mission attempt{initial};
            while (play(attempt, scores) == Input::Available && askRetry()) {
                attempt = initial;
            }
        }
    } catch (const std::exception& error) {
        std::cout << "Error: " << error.what() << '\n';
        return 1;
    }
    std::cout<<"End.\n";
    return 0;
}