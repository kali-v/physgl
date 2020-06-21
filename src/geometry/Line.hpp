/**
* Line
* format: y = m*x + c
*/
class Line
{
public:
    double m, c;
    bool vertical = false;
    Line() = default;

    Line(Coordinate a, Coordinate b)
    {
        m = (a.y - b.y) / (a.x - b.x);
        if (std::isinf(m))
        {
            c = a.x;
            vertical = true;
        }
        else
        {
            c = -m * a.x + a.y;
        }
    }

    Line(double _m, double _c, bool _vertical)
    {
        m = _m;
        c = _c;
        vertical = _vertical;
    }

    Line *get_perpendicular_line(Coordinate a, Coordinate b)
    {
        // get perpendicular line slope
        double pm = -1 / m;
        if (std::isinf(pm))
        {
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

    void print_line()
    {
        printf("m = %f; c = %f; vertical %s \n", m, c, (vertical) ? "true" : "false");
    }
};