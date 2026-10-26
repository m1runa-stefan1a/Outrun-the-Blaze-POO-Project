#include "HexCoord.h"

#include <array>
#include <cstddef>

namespace {
    struct Step {
        int dRow;
        int dCol;
    };
}
constexpr std::array<Step, 6> evenRowSteps{{
        {0, +1}, {-1, 0}, {-1, -1}, {0, -1}, {+1, -1}, {+1, 0}
}};
constexpr std::array<Step, 6> oddRowSteps{{
        {0, +1}, {-1, +1}, {-1, 0}, {0, -1}, {+1, 0}, {+1, +1}
}};

HexCoord::HexCoord(int row, int col) : row_{row}, col_{col} {}

HexCoord HexCoord::neighbour(Direction direction) const {
    const auto index = static_cast<std::size_t>(direction);
    const bool oddRow = (row_ % 2) != 0;
    const Step step = oddRow ? evenRowSteps[index] : oddRowSteps[index];
    return HexCoord{row_ + step.dRow, col_ + step.dCol};
}
bool HexCoord::operator==(const HexCoord& other) const {
    return row_ == other.row_ && col_ == other.col_;
}
std::ostream& operator<<(std::ostream& os, const HexCoord& coord) {
    os << "(" << coord.row_ << ", " << coord.col_ << ")";
    return os;
}