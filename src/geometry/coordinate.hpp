#ifndef COORDINATE_HPP
#define COORDINATE_HPP

#include <cmath>
#include <cstdio>

class Coordinate {
  public:
    float x, y, z;

    Coordinate();
    Coordinate(float _x, float _y, float _z);
    double get_distance_to(Coordinate b);
    bool is_near(Coordinate b);
    bool equals(Coordinate other);
    static bool cmp_by_x(Coordinate a, Coordinate b);
    static bool cmp_by_y(Coordinate a, Coordinate b);
    void print();
};

#endif