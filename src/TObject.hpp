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
    std::list<Line> collision_area;
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
        x_force = -0.002;
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
    }

    void create_circuit_ca()
    {
        collision_area.clear();
        int edge_cnt = vertices_list.size();
        for (int i = 0; i < edge_cnt; i += 3)
        {
            Coordinate a = Coordinate(
                get_point_at_index(i) + x,
                get_point_at_index(i + 1) + y,
                get_point_at_index(i + 2) + z);
            Coordinate b =
                (i + 3 == edge_cnt)
                    ? Coordinate(
                          get_point_at_index(0) + x,
                          get_point_at_index(1) + y,
                          get_point_at_index(2) + z)
                    : Coordinate(
                          get_point_at_index(i + 3) + x,
                          get_point_at_index(i + 4) + y,
                          get_point_at_index(i + 5) + z);

            Line *line = new Line(a, b);

            collision_area.push_back(*line);
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
                      ? x_force - AIR_FRICTION
                      : x_force + AIR_FRICTION;

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

    void get_edges(double *min_x, double *max_x, double *min_y, double *max_y)
    {
        Coordinate point = Coordinate(
            get_point_at_index(0),
            get_point_at_index(1),
            get_point_at_index(2));
        *min_x = point.x;
        *max_x = point.x;
        *min_y = point.y;
        *max_y = point.y;
        for (int i = 3; i < vertices_list.size(); i += 3)
        {
            point = Coordinate(
                get_point_at_index(i),
                get_point_at_index(i + 1),
                get_point_at_index(i + 2));
            if (point.x < *min_x)
                *min_x = point.x;
            if (point.x > *max_x)
                *max_x = point.x;
            if (point.y < *min_y)
                *min_y = point.y;
            if (point.y > *max_y)
                *max_y = point.y;
        }
        *min_x += x;
        *max_x += x;
        *min_y += y;
        *max_y += y;
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