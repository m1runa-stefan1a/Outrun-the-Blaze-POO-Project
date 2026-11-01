#ifndef MISSION_H
#define MISSION_H
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include "AirOpsDispatcher.h"
#include "ForestMap.h"
#include "HexCoord.h"
#include "WindConditions.h"

class Mission {
public:
    enum class Status{InProgress, Won, Lost};
    Mission(std::string name, const WindConditions& wind, ForestMap map, AirOpsDispatcher dispatcher, const std::vector<HexCoord>& fires, int timeLimit);
    Mission(const Mission& other);
    Mission& operator=(const Mission& other);
    ~Mission();
    void dispatch(std::size_t tankerIndex, const HexCoord& target);
    void recall(std::size_t tankerIndex);
    void advance();
    Status status() const;
    friend std::ostream& operator<<(std::ostream& os, const Mission& mission);
private:
    std::string name_;
    WindConditions wind_;
    ForestMap map_;
    AirOpsDispatcher dispatcher_;
    int timeLimit_;
    int elapsed_=0;
};
#endif