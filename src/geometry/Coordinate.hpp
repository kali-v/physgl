class Coordinate
{
public:
    float x, y, z;
    Coordinate() = default;

    Coordinate(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    double get_distance_to(Coordinate b)
    {
        return sqrt(pow(x - b.x, 2) + pow(y - b.y, 2) + pow(z - b.z, 2));
    }

    bool is_near(Coordinate b)
    {
        return get_distance_to(b) < 0.006;
    }

    bool equals(Coordinate other)
    {
        return (
            x == other.x &&
            y == other.y &&
            z == other.z);
    }

    static bool cmp_by_x(Coordinate a, Coordinate b)
    {
        return a.x < b.x;
    }
    static bool cmp_by_y(Coordinate a, Coordinate b)
    {
        return a.y < b.y;
    }

    void print() {
        printf("x: %f, y: %f\n", x, y);
    }
};