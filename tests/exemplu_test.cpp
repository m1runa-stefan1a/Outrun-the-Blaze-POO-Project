#undef NDEBUG
#include <cassert>

#include "HexCoord.h"

int main() {
    const HexCoord start{2, 2};
    assert(start.neighbour(Direction::East) == (HexCoord{2, 3}));
    for (const Direction direction : allDirections) {
        assert(start.distanceTo(start.neighbour(direction)) == 1);
    }
    assert(start.distanceTo(start) == 0);

    const HexCoord target{5, 7};
    assert(start.stepTowards(target).distanceTo(target) == start.distanceTo(target) - 1);

    return 0;
}