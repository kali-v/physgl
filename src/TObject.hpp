class TObject
{
public:
    int id;
    float col_r, col_g, col_b;
    double x, y, z;
    double x_force, y_force, z_force;
    double weight, friction, bounciness;

    float *vertices;
    std::list<float> vertices_list;
    std::list<Coordinate> collision_area;
    size_t vertices_size;

    TObject() = default;

    TObject(
        int _id,
        std::list<float> &_vertices,
        double _x, double _y, double _z,
        double _weight,
        double _friction,
        double _bounciness,
        float _col_r, float _col_g, float _col_b)
    {
        id = _id;
        //shape
        vertices_list = _vertices;
        vertices_size = _vertices.size() * sizeof(float);
        vertices = (float *)malloc(vertices_size);
        std::copy(_vertices.begin(), _vertices.end(), vertices);

        // init positions and forces
        x = _x;
        y = _y;
        z = _z;
        x_force = -0.001;
        y_force = -0.002;
        z_force = 0;

        // physical properties
        friction = _friction;
        weight = _weight;
        bounciness = _bounciness;

        // colors
        col_r = _col_r;
        col_g = _col_g;
        col_b = _col_b;

        create_triangle_collision_area(0.0015);
    }

    void create_triangle_collision_area(double density)
    {
        Coordinate a = Coordinate(
            get_point_at_index(0),
            get_point_at_index(1),
            get_point_at_index(2));
        Coordinate b = Coordinate(
            get_point_at_index(3),
            get_point_at_index(4),
            get_point_at_index(5));
        Coordinate c = Coordinate(
            get_point_at_index(6),
            get_point_at_index(7),
            get_point_at_index(8));

        Coordinate left_coor = std::min(a, b, Coordinate::compare_by_x);
        left_coor = std::min(left_coor, c, Coordinate::compare_by_x);
        Coordinate right_coor = std::max(a, b, Coordinate::compare_by_x);
        right_coor = std::max(right_coor, c, Coordinate::compare_by_x);

        Coordinate center_coor = a;
        if (!c.equals(left_coor) && !c.equals(right_coor))
            center_coor = c;
        else if (!b.equals(left_coor) && !b.equals(right_coor))
            center_coor = b;

        Line *lr_line = new Line(left_coor, right_coor);
        Line *lc_line = new Line(left_coor, center_coor);
        Line *rc_line = new Line(right_coor, center_coor);

        push_edge_to_collision_area(lr_line, &left_coor, &right_coor, density);
        push_edge_to_collision_area(lc_line, &left_coor, &center_coor, density);
        push_edge_to_collision_area(rc_line, &center_coor, &right_coor, density);

        free(lr_line);
        free(lc_line);
        free(rc_line);
    }

    void push_edge_to_collision_area(Line *line, Coordinate *left_coor, Coordinate *right_coor, double density)
    {
        for (float i = left_coor->x; i < right_coor->x; i += density)
        {
            Coordinate *c = nullptr;
            if (line->vertical)
                c = new Coordinate(line->c, i, 0);
            else
                c = new Coordinate(i, line->m * i + line->c, 0);
            collision_area.push_back(*c);
        }
    }

    float get_point_at_index(int index)
    {
        auto point_ptr = vertices_list.begin();
        std::advance(point_ptr, index);
        return *point_ptr;
    }

    void apply_y_force()
    {
        y_force -= (weight * G_FORCE);

        if (y < -1)
            y_force = 0;
    }

    void apply_x_force()
    {
        double abs_x_force = (x_force < 0) ? -x_force : x_force;
        if (abs_x_force < 0.00015)
        {
            x_force = 0;
            return;
        }
        bool force_dir = x_force > 0;

        x_force = (x_force > 0)
                      ? x_force - WIND_FRICTION
                      : x_force + WIND_FRICTION;

        if (x_force > 0 != force_dir)
            x_force = 0;
    }

    void check_ground_collision()
    {
        if (y < -1)
        {
            y = -1;

            double abs_y_force = (y_force < 0) ? -y_force : y_force;
            double abs_x_force = (x_force < 0) ? -x_force : x_force;
            if (abs_y_force > 0.00015)
            {
                y_force = -bounciness * y_force;
                if (x_force > 0)
                {
                    x_force = -1 * ((1 - friction) * (1 - SURF_FRICTION)) * (bounciness - 1) * y_force;
                }
                else
                {
                    x_force = ((1 - friction) * (1 - SURF_FRICTION)) * (bounciness - 1) * y_force;
                }
            }
            else
            {
                y_force = 0;
            }
        }
    }

    void update_position()
    {
        check_ground_collision();
        apply_y_force();
        apply_x_force();

        x += x_force;
        y += y_force;
    }
};