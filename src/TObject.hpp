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

        create_circuit_ca(vertices_list.size());
    }

    void create_circuit_ca(int edge_cnt)
    {
        for (int i = 0; i < edge_cnt; i += 3)
        {
            Coordinate a = Coordinate(
                get_point_at_index(i),
                get_point_at_index(i + 1),
                get_point_at_index(i + 2));
            Coordinate b =
                (i + 3 == edge_cnt)
                    ? Coordinate(
                          get_point_at_index(0),
                          get_point_at_index(1),
                          get_point_at_index(2))
                    : Coordinate(
                          get_point_at_index(i + 3),
                          get_point_at_index(i + 4),
                          get_point_at_index(i + 5));

            Line *line = new Line(a, b);

            push_line_to_ca(line, &a, &b);
            free(line);
        }
    }

    void push_line_to_ca(Line *line, Coordinate *a, Coordinate *b)
    {
        double distance = a->get_distance_to(*b);
        double density = distance / (distance * 1000.0 / 2.0);

        std::function<bool(Coordinate, Coordinate)> cmp_coord =
            (line->vertical) ? Coordinate::cmp_by_y : Coordinate::cmp_by_x;

        float min_val;
        float max_val;

        if (cmp_coord(*a, *b))
        {
            min_val = (line->vertical) ? a->y : a->x;
            max_val = (line->vertical) ? b->y : b->x;
        }
        else
        {
            min_val = (line->vertical) ? b->y : b->x;
            max_val = (line->vertical) ? a->y : a->x;
        }

        for (float i = min_val; i < max_val; i += density)
        {
            Coordinate *c =
                (line->vertical)
                    ? new Coordinate(line->c, i, 0)
                    : new Coordinate(i, line->m * i + line->c, 0);
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