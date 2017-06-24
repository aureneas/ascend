#include "../include/point.h"

bool Point::operator==(Point& p) {
    return (x == p.x && y == p.y);
}

Point Point::operator-(Point& p) {
    return { x - p.x, y - p.y };
}

void Point::operator+=(Point& p) {
    x += p.x;
    y += p.y;
}

Point Point::operator*(int m) {
    return { m * x, m * y };
}
