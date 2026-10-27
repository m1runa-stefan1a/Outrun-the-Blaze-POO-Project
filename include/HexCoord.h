#ifndef HEX_COORD_H
#define HEX_COORD_H
#include <ostream>
#include <cstddef>
#include "Direction.h"


class HexCoord {
    private:
        int row_;
        int col_;
    public:
        HexCoord(int row, int col);
        HexCoord neighbour(Direction direction) const;
        int distanceTo(const HexCoord& other) const;
        HexCoord stepTowards(const HexCoord& target) const;
        bool isInside(int rowCount, int colCount) const;
        std::size_t indexIn(int colCount) const;
        bool operator==(const HexCoord& other) const;
        friend std::ostream& operator<<(std::ostream& os, const HexCoord& coord);
};
#endif