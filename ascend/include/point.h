#ifndef A_POINT_H
#define A_POINT_H

struct Point {
    int x;
    int y;

    bool operator==(Point&);
    const Point& operator-(Point&);
    void operator+=(Point&);
};

#endif // A_POINT_H
