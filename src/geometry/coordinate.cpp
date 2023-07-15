#include "coordinate.hpp"

Coordinate::Coordinate() = default;

Coordinate::Coordinate(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
}

double Coordinate::get_distance_to(Coordinate b) {
    return std::sqrt(std::pow(x - b.x, 2) + std::pow(y - b.y, 2) + std::pow(z - b.z, 2));
}

bool Coordinate::is_near(Coordinate b) { return get_distance_to(b) < 0.006; }

bool Coordinate::equals(Coordinate other) { return (x == other.x && y == other.y && z == other.z); }

bool Coordinate::cmp_by_x(Coordinate a, Coordinate b) { return a.x < b.x; }

bool Coordinate::cmp_by_y(Coordinate a, Coordinate b) { return a.y < b.y; }

void Coordinate::print() { printf("x: %f, y: %f\n", x, y); }