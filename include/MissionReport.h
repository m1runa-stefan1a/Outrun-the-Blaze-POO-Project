#ifndef MISSION_REPORT_H
#define MISSION_REPORT_H
#include <ostream>
#include <string>

class MissionReport {
public:
    MissionReport(std::string missionName, bool won, int burnedSectors, int crashedTankers, int remainingSteps);
    int score() const;
    friend std::ostream& operator<<(std::ostream& os, const MissionReport& report);
private:
    std::string missionName_;
    bool won_;
    int burnedSectors_;
    int crashedTankers_;
    int remainingSteps_;
};
#endif