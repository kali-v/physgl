#include "line.hpp"
#include <algorithm>
#include <cstdio>

/**
 * Line
 * format: y = m*x + c
 */

Line::Line(Coordinate _a, Coordinate _b) {
    a = _a;
    b = _b;
    m = (a.y - b.y) / (a.x - b.x);
    if (std::isinf(m)) {
        c = a.x;
        vertical = true;
    } else {
        c = -m * a.x + a.y;
    }
}

Line::Line(double _m, double _c, bool _vertical) {
    m = _m;
    c = _c;
    vertical = _vertical;
}

Line* Line::get_perpendicular_line(Coordinate a, Coordinate b) {
    // get perpendicular line slope
    double pm = -1 / m;
    if (std::isinf(pm)) {
        return new Line(INFINITY, (a.x + b.x) / 2, true);
    }

    // get center between a and b
    double max_x = std::max(a.x, b.x);
    double min_x = std::min(a.x, b.x);
    double max_y = std::max(a.y, b.y);
    double min_y = std::min(a.y, b.y);
    double cx = (max_x - min_x) / 2;
    double cy = (max_y - min_y) / 2;

    // get c of perpendicular line
    double pc = -pm * (max_x - cx) + (max_y - cy);
    return new Line(pm, pc, false);
}

double* Line::get_vertical_segment_intersect(double x_inter, double y_inter, double ay, double by) {
    if (y_inter < std::max(ay, by) && y_inter > std::min(ay, by)) {
        double* inter = (double*)malloc(sizeof(double) * 2);
        inter[0] = x_inter;
        inter[1] = y_inter;
        return inter;
    }
    return nullptr;
}

double* Line::get_segment_intersect(Line lineSegment) {
    double xs[4] = {a.x, b.x, lineSegment.a.x, lineSegment.b.x};
    double ys[4] = {a.y, b.y, lineSegment.a.y, lineSegment.b.y};

    if (vertical || lineSegment.vertical) {
        double y_inter = vertical ? lineSegment.m * c + lineSegment.c : m * lineSegment.c + c;

        return get_vertical_segment_intersect((vertical) ? c : lineSegment.c, y_inter, (vertical) ? ys[2] : ys[0],
                                              (vertical) ? ys[3] : ys[1]);
    }
    if (m == lineSegment.m) return nullptr;

    double x_inter = (lineSegment.c - c) / (m - lineSegment.m);
    if (x_inter < std::max(std::min(xs[0], xs[1]), std::min(xs[2], xs[3])) ||
        x_inter > std::min(std::max(xs[0], xs[1]), std::max(xs[2], xs[3]))) {
        return nullptr;
    }

    double y_inter = m * x_inter + c;
    double* inter = (double*)malloc(sizeof(double) * 2);
    inter[0] = x_inter;
    inter[1] = y_inter;
    return inter;
}

void Line::print_line() { printf("m = %f; c = %f; vertical %s \n", m, c, (vertical) ? "true" : "false"); }