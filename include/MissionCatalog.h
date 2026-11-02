#ifndef MISSION_CATALOG_H
#define MISSION_CATALOG_H
#include <cstddef>
#include <string>
#include <vector>

#include "Mission.h"

class MissionCatalog {
public:
    explicit MissionCatalog(const std::string& path);
    std::size_t size() const;
    const Mission& at(std::size_t index) const;
private:
    std::vector<Mission> missions_;
};
#endif