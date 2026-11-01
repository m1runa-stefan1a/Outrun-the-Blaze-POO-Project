#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "Mission.h"

namespace {
    enum class Input { Available, Ended };
    Mission buildMission() {
        const ForestMap map{{"PPPPPPPPPP",
                         "PPPPPPPPPP",
                         "PPLLPPPPPP",
                         "PPPPPPPPPP",
                         "PPPPPPPPCP",
                         "PPPPPPPPPP"}, 60};
        const AirOpsDispatcher dispatcher{{AirTanker{"Dipper", 3000, 60, HexCoord{3, 1}},
                                       AirTanker{"Cabbie", 1500, 30, HexCoord{3, 1}}},
                                      1500, 1000};
        return Mission{"Lacul Linistit", WindConditions{Direction::East, 35}, map, dispatcher,
                       {HexCoord{2, 5}}, 30};
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
        std::cout<<"Aeronava " << tanker << " pleaca spre " << target << '\n';
    } catch (const std::out_of_range& error) {
        std::cout << "Comanda invalida: " << error.what() << '\n';
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
        std::cout<<"command: send <palne> <row> <col> | wait <steps> :\n";
        std::string command;
        if (!(std::cin >> command)) {
            return Input::Ended;
        }
        Input input=Input::Available;
        if (command == "send") {
            input=readDispatch(mission);
        } else if (command == "wait") {
            input=readWait(mission);
        } else {
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
    const Mission initial = buildMission();
    Mission attempt{initial};
    while (play(attempt)==Input::Available && askRetry()) {
        attempt = initial;
    }
    std::cout<<"End.\n";
    return 0;
}