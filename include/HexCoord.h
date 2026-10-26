#ifndef HEX_COORD_H
#define HEX_COORD_H
#include <ostream>

#include "Direction.h"


class HexCoord {
    private:
        int row_;
        int col_;
    public:
        HexCoord(int row, int col);
        HexCoord neighbour(Direction direction) const;
        bool isInside(int rowCount, int colCount) const;
        bool operator==(const HexCoord& coord) const;
        friend std::ostream& operator<<(std::ostream& os, const HexCoord& coord);
};
#endif