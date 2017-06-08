#include "../include/point.h"

bool Point::operator==(Point& p) {
    return (x == p.x && y == p.y);
}

const Point& Point::operator-(Point& p) {
    Point temp = { x - p.x, y - p.y };
    return temp;
}

void Point::operator+=(Point& p) {
    x += p.x;
    y += p.y;
}
