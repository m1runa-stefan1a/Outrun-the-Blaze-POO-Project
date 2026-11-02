#include "ForestMap.h"
#include <algorithm>
#include <iomanip>
#include <stdexcept>

namespace {
    constexpr int cellWidth = 4;
    WildfireSector::Terrain terrainFromSymbol(char symbol) {
        switch (symbol) {
            case 'P': return WildfireSector::Terrain::Forest;
            case 'L': return WildfireSector::Terrain::Lake;
            case 'C': return WildfireSector::Terrain::Cabin;
            default:
                throw std::invalid_argument(std::string{"ForestMap: unknown terrain '"}+symbol+"'");
        }
    }

int heatToward(int heat, Direction toward, const WindConditions& wind) {
        const int speed = wind.speedKmh();
        int percent = 100 - speed;
        if (toward == wind.blowsToward()) {
            percent = 100 + 2 * speed;
        } else if (toward == opposite(wind.blowsToward())) {
            percent = 100 - 2 * speed;
        }
        return heat * std::max(0,percent)/100;
    }
};
ForestMap::ForestMap(const std::vector<std::string>& layout, int humidityPercent)
    :rowCount_{static_cast<int>(layout.size())},
    colCount_{layout.empty()?0:static_cast<int>(layout.front().size())},
    humidityPercent_{humidityPercent} {
    if (rowCount_ == 0 || colCount_ == 0) {
        throw std::invalid_argument("ForestMap: empty map");
    }
    if (humidityPercent_ <0 || humidityPercent > 100) {
        throw std::invalid_argument("ForestMap: humidity must be between 0 and 100");
    }
    for (int row =0; row < rowCount_; ++row) {
        const std::string& line = layout[static_cast<std::size_t>(row)];
        if (static_cast<int>(line.size()) != colCount_) {
            throw std::invalid_argument("ForestMap: invalid map layout");
        }
        for (int col = 0; col<colCount_; ++col) {
            sectors_.emplace_back(HexCoord{row, col}, terrainFromSymbol(line[static_cast<std::size_t>(col)]));
        }
    }
}
void ForestMap::ignite(const HexCoord& where) {
    sectors_[indexOf(where)].ignite();
}

bool ForestMap::contains(const HexCoord &where) const {
    return where.isInside(rowCount_, colCount_);
}

void ForestMap::spreadFire(const WindConditions& wind) {
    std::vector<int> incoming(sectors_.size(),0);
    for ( const WildfireSector& sector : sectors_) {
        const int heat = sector.heatOutput(humidityPercent_);
        if (heat == 0) {
            continue;
        }
        for (const Direction direction : allDirections) {
            const HexCoord target = sector.position().neighbour(direction);
            if (target.isInside(rowCount_, colCount_)) {
                incoming[target.indexIn(colCount_)] += heatToward(heat, direction, wind);

            }
        }
    }
    for (std::size_t i=0; i<sectors_.size(); ++i) {
        sectors_[i].addHeat(incoming[i]);
        sectors_[i].advanceFire();
    }
}

void ForestMap::douse(const HexCoord& where, int liters) {
    sectors_[indexOf(where)].douse(liters);
}

bool ForestMap::isLake(const HexCoord& where) const {
    return sectors_[indexOf(where)].isLake();
}
bool ForestMap::isBurning(const HexCoord& where) const {
    return sectors_[indexOf(where)].isBurning();
}
bool ForestMap::hasActiveFire() const {
    return std::any_of(sectors_.begin(), sectors_.end(),
        [](const WildfireSector& sector) { return sector.isBurning(); });
}
int ForestMap::lostPercent() const {
    const auto burnable = std::count_if(sectors_.begin(), sectors_.end(),
        [](const WildfireSector& sector) { return !sector.isLake(); });
    if (burnable==0) {
        return 0;
    }
    const auto lost = std::count_if(sectors_.begin(), sectors_.end(),
        [](const WildfireSector& sector) { return sector.isLost(); });
    return static_cast<int>(lost*100/burnable);
}
int ForestMap::lostCount() const {
    return static_cast<int>(std::count_if(sectors_.begin(), sectors_.end(),
        [](const WildfireSector& sector) { return sector.isLost(); }));
}

bool ForestMap::isCabinLost() const {
    return std::any_of(sectors_.begin(), sectors_.end(),
        [](const WildfireSector& sector) { return sector.isCabin() && sector.isLost(); });
}
int ForestMap::burningCount() const {
    return static_cast<int>(std::count_if(sectors_.begin(), sectors_.end(),
        [](const WildfireSector& sector) { return sector.isBurning(); }));
}

bool ForestMap::isCabinThreatened() const {
    for (const WildfireSector& sector : sectors_) {
        if (!sector.isCabin()||sector.isLost()) {
            continue;
        }
        for (const Direction direction : allDirections) {
            const HexCoord near = sector.position().neighbour(direction);
            if (contains(near) && isBurning(near)) {
                return true;
            }
        }
    }
    return false;
}

std::size_t ForestMap::indexOf(const HexCoord& where) const {
    if (!contains(where)) {
        throw std::out_of_range("ForestMap: position is not on the map");
    }
    return where.indexIn(colCount_);
}

std::ostream& operator<<(std::ostream& os, const ForestMap& map) {
    os << "     ";
    for (int col = 0; col < map.colCount_; ++col) {
        os << std::left << std::setw(cellWidth) << col;
    }
    for (std::size_t i=0; i<map.sectors_.size(); ++i) {
        const auto row = static_cast<int>(i)/map.colCount_;
        const bool rowStart = static_cast<int>(i)%map.colCount_ == 0;
        if (rowStart) {
            os << '\n' << std::right << std::setw(2) << row << "   ";
            if (row%2!=0) {
                os << std::string(cellWidth / 2, ' ');
            }
        }
        os << std::left << std::setw(cellWidth) << map.sectors_[i];
    }
    os << std::right << "\nhumidity " << map.humidityPercent_ << "%, lost " << map.lostPercent() << "%";
    return os;
}
