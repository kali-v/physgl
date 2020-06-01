class TObject
{
public:
    float col_r, col_g, col_b;
    double x, y, z;
    double x_force, y_force, z_force;
    double weight, friction, bounciness;

    float *vertices;
    std::list<float> vertices_list;
    size_t vertices_size;

    TObject() = default;

    TObject(
        std::list<float> &_vertices,
        double _x, double _y, double _z,
        double _weight,
        double _friction,
        double _bounciness,
        float _col_r, float _col_g, float _col_b)
    {
        //shape
        vertices_list = _vertices;
        vertices_size = _vertices.size() * sizeof(float);
        vertices = (float *) malloc(vertices_size);
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