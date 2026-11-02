#ifndef SCORE_BOARD_H
#define SCORE_BOARD_H
#include <map>
#include <string>

#include "MissionReport.h"

class ScoreBoard {
public:
    explicit ScoreBoard(std::string path);
    int bestFor(const std::string& missionName) const;
    bool submit(const MissionReport& report);
private:
    void load();
    void save() const;
    std::string path_;
    std::map<std::string, int> best_;
};
#endif