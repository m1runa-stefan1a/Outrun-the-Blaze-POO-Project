#include "MissionReport.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace {
    constexpr int startingScore = 1000;
    constexpr int burnedSectorPenalty = 20;
    constexpr int crashedTankerPenalty = 150;
    constexpr int remainingStepsBonus = 10;
}

MissionReport::MissionReport(std::string missionName, bool won, int burnedSectors, int crashedTankers, int remainingSteps)
    :missionName_{std::move(missionName)}, won_{won}, burnedSectors_{burnedSectors}, crashedTankers_{crashedTankers}, remainingSteps_{remainingSteps} {
    if (burnedSectors_ < 0 || crashedTankers_ < 0 || remainingSteps_ < 0) {
        throw std::invalid_argument("MissionReport: counts can't be negative");
    }
}
 int MissionReport::score() const {
    if (!won_) {
        return 0;
    }
    const int score = startingScore
                    - burnedSectors_ * burnedSectorPenalty
                    - crashedTankers_ * crashedTankerPenalty
                    + remainingSteps_ * remainingStepsBonus;
    return std::max(0, score);
}

std::ostream& operator<< (std::ostream& os, const MissionReport& report) {
    os<< "=== MISSION REPORT: "<< report.missionName_ <<" ===\n";
    if (!report.won_) {
        os<< "result: Lost - score 0\n";
        return os;
    }
    os<<"result: Won\n"<< "starting score: " << startingScore << '\n'
        <<"burned sectors: " << report.burnedSectors_ << " x -"<< burnedSectorPenalty<<'\n'
        <<"crashed aircraft: "<< report.crashedTankers_ << " x -"<< crashedTankerPenalty<<'\n'
        <<"remaining steps: "<<report.remainingSteps_<< " x +" << remainingStepsBonus<< '\n'
        <<"SCORE: "<<report.score()<<'\n';
    return os;
}