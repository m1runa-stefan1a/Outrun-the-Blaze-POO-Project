#include "Direction.h"

Direction opposite(Direction direction) {
    const auto index = static_cast<std::size_t>(direction);
    return allDirections[(index+3)%allDirections.size()];
}
std::ostream& operator<<(std::ostream& os, Direction direction) {
    switch (direction) {
        case Direction::East:      os << "E";  break;
        case Direction::NorthEast: os << "NE"; break;
        case Direction::NorthWest: os << "NW"; break;
        case Direction::West:      os << "W";  break;
        case Direction::SouthWest: os << "SW"; break;
        case Direction::SouthEast: os << "SE"; break;
    }
    return os;
}
