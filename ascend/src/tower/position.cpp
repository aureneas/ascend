#include "../../include/point.h"
#include "../../include/tower/position.h"

namespace tower {

Point translate(Point p, Direction d) {
    Point n = { p.x, p.y };

    if (d == EAST)
        n.x += 18;
    else if (d == WEST)
        n.x -= 18;

    if (d == SOUTH)
        n.y += 18;
    else if (d == NORTH)
        n.y -= 18;

    return n;
}

}
