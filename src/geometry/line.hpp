#ifndef LINE_HPP
#define LINE_HPP

#include <cmath>
#include "coordinate.hpp"

class Line {
  public:
    double m, c;
    Coordinate a, b;
    bool vertical = false;

    Line();
    Line(Coordinate _a, Coordinate _b);
    Line(double _m, double _c, bool _vertical);
    Line* get_perpendicular_line(Coordinate a, Coordinate b);
    double* get_vertical_segment_intersect(double x_inter, double y_inter, double ay, double by);
    double* get_segment_intersect(Line lineSegment);
    void print_line();
};

#endif