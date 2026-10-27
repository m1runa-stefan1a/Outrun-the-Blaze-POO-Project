#include "Direction.h"

Direction oposite(Direction direction) {
    const auto index = static_cast<std::size_t>(direction);
    return allDirections[(index+3)%allDirections.size()];
}
std::ostream& operator<<(std::ostream& os, Direction direction) {
    switch (direction) {
        case Direction::East:      os << "E";  break;
        case Direction::NorthEast: os << "NE"; break;
        case Direction::NorthWest: os << "NV"; break;
        case Direction::West:      os << "V";  break;
        case Direction::SouthWest: os << "SV"; break;
        case Direction::SouthEast: os << "SE"; break;
    }
    return os;
}
