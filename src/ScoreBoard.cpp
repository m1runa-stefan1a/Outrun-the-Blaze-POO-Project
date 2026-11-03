#include "ScoreBoard.h"
#include <fstream>
#include <stdexcept>
#include <utility>

#include <json/json.hpp>

ScoreBoard::ScoreBoard(std::string path)
    : path_{std::move(path)} {
    load();
}

int ScoreBoard::bestFor(const std::string& missionName) const {
    const auto found = best_.find(missionName);
    if (found == best_.end()) {
        return 0;
    }
    return found->second;
}

bool ScoreBoard::submit(const MissionReport& report) {
    if (report.score() <= bestFor(report.missionName())) {
        return false;
    }
    best_[report.missionName()] = report.score();
    save();
    return true;
}

void ScoreBoard::load() {
    std::ifstream file{path_};
    if (!file) {
        return;
    }
    try {
        best_ = nlohmann::json::parse(file).get<std::map<std::string, int>>();
    } catch (const nlohmann::json::exception&) {
        throw std::runtime_error("ScoreBoard: the scores file is corrupted: " + path_);
    }
}

void ScoreBoard::save() const {
    std::ofstream file{path_};
    if (!file) {
        throw std::runtime_error("ScoreBoard: cannot write the scores file: " + path_);
    }
    const nlohmann::json data = best_;
    file << data.dump(2) << '\n';
}
std::ostream& operator<<(std::ostream& os, const ScoreBoard& board) {
    os << "=== HIGH SCORES ===";
    if (board.best_.empty()) {
        os << "\nno records yet";
    }
    for (const auto& record : board.best_) {
        os << '\n' << record.first << ": " << record.second;
    }
    return os;
}