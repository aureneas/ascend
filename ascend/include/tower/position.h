#ifndef AT_POSITION_H
#define AT_POSITION_H

#include "../types.h"

namespace tower {

struct Position {
    u_16 floor;
    u_16 tile;

    Position() : floor(0), tile(0) {}
    Position(u_16 f, u_16 t) : floor(f), tile(t) {}
};

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

Point translate(Point, Direction);

}

#endif // AT_POSITION_H
