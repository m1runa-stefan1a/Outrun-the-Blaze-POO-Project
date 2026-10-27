#include "HexCoord.h"
#include <cstdlib>
#include <array>
#include <cstddef>

namespace {
    struct Step {
        int dRow;
        int dCol;
    };
constexpr std::array<Step, 6> evenRowSteps{{
        {0, +1}, {-1, 0}, {-1, -1}, {0, -1}, {+1, -1}, {+1, 0}
}};
constexpr std::array<Step, 6> oddRowSteps{{
        {0, +1}, {-1, +1}, {-1, 0}, {0, -1}, {+1, 0}, {+1, +1}
}};
}

HexCoord::HexCoord(int row, int col) : row_{row}, col_{col} {}

HexCoord HexCoord::neighbour(Direction direction) const {
    const auto index = static_cast<std::size_t>(direction);
    const bool oddRow = (row_ % 2) != 0;
    const Step step = oddRow ? oddRowSteps[index] : evenRowSteps[index];
    return HexCoord{row_ + step.dRow, col_ + step.dCol};
}

int HexCoord::distanceTo(const HexCoord& other) const {
    const int q1 = col_ - (row_ - (row_ & 1)) / 2;
    const int q2 = other.col_ - (other.row_ - (other.row_ & 1)) / 2;
    const int dq = q1 - q2;
    const int dr = row_ -other.row_;
    const int ds = -dq -dr;
    return (std::abs(dq) + std::abs(dr) + std::abs(ds)) / 2;
}
HexCoord HexCoord::stepTowards(const HexCoord& target) const {
    HexCoord best = *this;
    int bestDistance = distanceTo(target);
    for (const Direction direction : allDirections) {
        const HexCoord candidate = neighbour(direction);
        const int candidateDistance = candidate.distanceTo(target);
        if (candidateDistance < bestDistance) {
            best = candidate;
            bestDistance = candidateDistance;
        }
    }
    return best;
}
std::size_t HexCoord::indexIn(int colCount) const {
    return static_cast<std::size_t>(row_)*static_cast<std::size_t>(colCount)+static_cast<std::size_t>(col_);
}
bool HexCoord::isInside(int rowCount, int colCount) const {
    return row_ >= 0 && row_ < rowCount && col_ >= 0 && col_ < colCount;
}
bool HexCoord::operator==(const HexCoord& other) const {
    return row_ == other.row_ && col_ == other.col_;
}
std::ostream& operator<<(std::ostream& os, const HexCoord& coord) {
    os << "(" << coord.row_ << ", " << coord.col_ << ")";
    return os;
}