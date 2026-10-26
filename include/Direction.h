#ifndef DIRECTION_H
#define DIRECTION_H
#include <ostream>

enum class Direction { East, NorthEast, NorthWest, West, SouthWest, SouthEast };
std::ostream& operator<<(std::ostream& os, Direction direction);

#endif