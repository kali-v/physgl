class TObject
{
public:
    int id;
    float col_r, col_g, col_b;
    double x_force, y_force, z_force;
    double weight, friction, bounciness;
    float rotation_force;

    float *vertices;
    std::list<float> vertices_list;
    std::list<Line> collision_area;
    int vertices_count;

    TObject() = default;

    TObject(
        int _id,
        std::list<float> &_vertices,
        double _weight,
        double _friction,
        double _bounciness,
        //double _rotation,
        //double _rotation_force,
        float _col_r, float _col_g, float _col_b)
    {
        id = _id;

        vertices_count = _vertices.size();
        vertices = (float *)malloc(vertices_count * sizeof(float));
        std::copy(_vertices.begin(), _vertices.end(), vertices);

        // init positions and forces
        x_force = -0.002;
        y_force = -0.002;
        z_force = 0;

        rotation_force = 5;

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
        for (int i = 0; i < vertices_count; i += 3)
        {
            Coordinate a = Coordinate(
                vertices[i],
                vertices[i + 1],
                vertices[i + 2]);
            Coordinate b =
                (i + 3 == vertices_count)
                    ? Coordinate(
                          vertices[0],
                          vertices[1],
                          vertices[2])
                    : Coordinate(
                          vertices[i + 3],
                          vertices[i + 4],
                          vertices[i + 5]);

            Line *line = new Line(a, b);

            collision_area.push_back(*line);
        }
    }

    void check_ground_collision()
    {
        for (int i = 1; i < vertices_count; i += 3)
        {
            double cy = vertices[i];
            if (cy < -1)
            {
                rotation_force /= 1.5;
                for (int j = 1; j <= vertices_count; j += 3)
                {
                    vertices[j] -= 1 + cy;
                }
                double abs_y_force = (y_force < 0) ? -y_force : y_force;
                double abs_x_force = (x_force < 0) ? -x_force : x_force;
                if (abs_y_force > 0.0003)
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
    }

    void apply_y_force()
    {
        y_force -= (weight * G_FORCE);
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

    void apply_rotation_force()
    {
        rotation_force /= 1.01;
        if (rotation_force < 0.00015)
        {
            rotation_force = 0;
        }

        Coordinate point = get_center_of_gravity();

        glm::mat4 rotation_matrix = glm::mat4(1.0f);
        rotation_matrix = glm::translate(rotation_matrix, glm::vec3(point.x, point.y, point.z));
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation_force), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation_matrix = glm::translate(rotation_matrix, glm::vec3(-point.x, -point.y, -point.z));

        apply_matrix(rotation_matrix);
    }

    void apply_matrix(glm::mat4 model)
    {
        for (int i = 0; i < vertices_count; i += 3)
        {
            glm::vec4 cvec = glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1);
            cvec = model * cvec;
            vertices[i] = cvec.x;
            vertices[i + 1] = cvec.y;
            vertices[i + 2] = cvec.z;
        }
    }

    Coordinate get_center_of_gravity()
    {
        float x_sum, y_sum, z_sum = 0;
        for (int i = 0; i < vertices_count; i += 3)
        {
            x_sum += vertices[i];
            y_sum += vertices[i + 1];
            z_sum += vertices[i + 2];
        }

        return Coordinate(
            x_sum / (vertices_count / 3),
            y_sum / (vertices_count / 3),
            z_sum / (vertices_count / 3));
    }

    void update_position()
    {
        check_ground_collision();
        apply_rotation_force();
        apply_y_force();
        apply_x_force();

        for (int i = 0; i < vertices_count; i += 3)
        {
            vertices[i] += x_force;
            vertices[i + 1] += y_force;
        }
    }
};