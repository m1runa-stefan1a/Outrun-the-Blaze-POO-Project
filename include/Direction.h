#ifndef DIRECTION_H
#define DIRECTION_H
#include <ostream>
#include <array>

enum class Direction { East, NorthEast, NorthWest, West, SouthWest, SouthEast };
std::ostream& operator<<(std::ostream& os, Direction direction);

inline constexpr std::array<Direction, 6> allDirections{
    Direction::East, Direction::NorthEast, Direction::NorthWest, Direction::West,  Direction::SouthWest, Direction::SouthEast
};
Direction opposite(Direction direction);
std::ostream& operator<<(std::ostream& os, Direction direction);
#endif