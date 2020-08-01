class RigidBody
{
public:
    int id;
    float col_r, col_g, col_b;
    double x_force, y_force, z_force;
    double mass, friction, bounciness;
    float rotation_force;
    bool fixed;

    float *vertices;
    std::list<Line> collision_area;
    int vertices_count;

    int last_collision = 0;
    int last_ground_collision = 0;

    RigidBody() = default;

    RigidBody(
        int _id,
        std::list<float> &_vertices,
        double _mass,
        double _friction,
        double _bounciness)
    {
        id = _id;

        vertices_count = _vertices.size();
        vertices = (float *)malloc(vertices_count * sizeof(float));
        std::copy(_vertices.begin(), _vertices.end(), vertices);

        // physical properties
        friction = _friction;
        mass = _mass;
        bounciness = _bounciness;

        fixed = false;

        x_force = 0;
        y_force = 0;
        z_force = 0;
        rotation_force = 0;

        col_r = 127;
        col_g = 127;
        col_b = 127;
    }

    void set_forces(
        double _x_force,
        double _y_force,
        double _z_force,
        double _rotation_force)
    {
        x_force = _x_force;
        y_force = _y_force;
        z_force = _z_force;
        rotation_force = _rotation_force;
    }

    void set_fixed(bool _fixed)
    {
        fixed = _fixed;
    }

    void set_positon(
        double x,
        double y,
        double z)
    {
        for (int i = 0; i < vertices_count; i += 3)
        {
            vertices[i] += x;
            vertices[i + 1] += y;
            vertices[i + 2] += z;
        }
    }

    void set_color(
        float _col_r,
        float _col_g,
        float _col_b)
    {
        col_r = _col_r;
        col_g = _col_g;
        col_b = _col_b;
    }

    void create_circuit_ca()
    {
        collision_area.clear();
        for (int i = 0; i < vertices_count; i += 3)
        {
            Coordinate *a = new Coordinate(
                vertices[i],
                vertices[i + 1],
                vertices[i + 2]);
            Coordinate *b =
                (i + 3 == vertices_count)
                    ? new Coordinate(
                          vertices[0],
                          vertices[1],
                          vertices[2])
                    : new Coordinate(
                          vertices[i + 3],
                          vertices[i + 4],
                          vertices[i + 5]);

            Line *line = new Line(*a, *b);
            delete a;
            delete b;
            collision_area.push_back(*line);
        }
    }

    bool check_ground_collision()
    {
        bool left_touch = false;
        bool right_touch = false;
        for (int i = 1; i < vertices_count; i += 3)
        {
            float cy = vertices[i];
            if (cy < -1)
            {
                float diff_x = get_x_center_of_gravity() - vertices[i - 1];
                if (left_touch || right_touch)
                {
                    rotation_force = 0;
                }
                else if (diff_x > 0.0001)
                {
                    left_touch = true;
                    rotation_force /= 1.1;
                    rotation_force -= 0.05 + (diff_x / 5);
                }
                else if (diff_x < -0.0001)
                {
                    right_touch = true;
                    rotation_force /= 1.1;
                    rotation_force += 0.05 + (diff_x / 5);
                }

                for (int j = 1; j <= vertices_count; j += 3)
                {
                    vertices[j] -= 1 + cy;
                }

                x_force = x_force * (1 - (friction / 5));

                if (fabs(y_force) > 0.0003)
                {
                    y_force = -bounciness * y_force;
                    if (x_force > 0)
                    {
                        x_force = -1 * ((1 - friction) * (1 - SURF_FRICTION)) * (bounciness - 1) * (y_force * x_force);
                    }
                    else
                    {
                        x_force = ((1 - friction) * (1 - SURF_FRICTION)) * (bounciness - 1) * (y_force * x_force);
                    }
                }
                else
                {
                    y_force = 0;
                }
            }
        }
        if (left_touch || right_touch)
        {
            last_ground_collision = 0;
            return true;
        }
        return false;
    }

    void apply_elastic_collision(RigidBody *other_body)
    {
        double sw = (mass + other_body->mass);

        double xa = (mass - other_body->mass) / sw;
        double xb = (2 * other_body->mass) / sw;

        x_force = (xa * x_force) + (xb * other_body->x_force);
        y_force = (xa * y_force) + (xb * other_body->y_force);

        xa = (2 * mass) / sw;
        xb = (other_body->mass - mass) / sw;

        other_body->x_force = (xa * x_force) + (xb * other_body->x_force);
        other_body->y_force = (xa * y_force) + (xb * other_body->y_force);
    }

    void collide(RigidBody *other_body, double x, double y)
    {
        if (last_collision > COLLISION_DELAY)
        {
            double diff_y =
                (y_force > other_body->y_force)
                    ? y_force - other_body->y_force
                    : other_body->y_force - y_force;

            double bounce = other_body->bounciness * bounciness * -1;

            if (other_body->last_ground_collision < 3 || other_body->fixed)
            {
                x_force *= bounce;
                y_force *= bounce;
            }
            else if (last_ground_collision < 3 || fixed)
            {
                other_body->x_force *= bounce;
                other_body->y_force *= bounce;
            }
            else
            {
                apply_elastic_collision(other_body);
            }

            float diff_x = get_x_center_of_gravity() - x;
            if (diff_x > 0.0001)
            {
                x_force += diff_x / 100;
                rotation_force /= 1.05;
                rotation_force -= 0.035 + (diff_x / 2) + (diff_y * 10);
            }
            else if (diff_x < -0.0001)
            {
                x_force += diff_x / 100;
                rotation_force /= 1.05;
                rotation_force += 0.035 + (diff_x / 2)  + (diff_y * 10);
            }
            last_collision = 0;
        }
    }

    void calculate_y_force()
    {
        y_force -= 0.5 * G_FORCE;
    }

    void calculate_x_force()
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

    void calculate_rotation_force()
    {
        bool rotation_dir = rotation_force > 0;
        double rotation_friction = AIR_FRICTION * 50;
        rotation_force += (rotation_dir) ? -rotation_friction : rotation_friction;
        if (fabs(rotation_force) <= 0.003 || rotation_dir != rotation_force > 0)
        {
            rotation_force = 0;
        }

        Coordinate *point = get_center_of_gravity();

        glm::mat4 rotation_matrix = glm::mat4(1.0f);
        rotation_matrix = glm::translate(rotation_matrix, glm::vec3(point->x, point->y, point->z));
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation_force), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation_matrix = glm::translate(rotation_matrix, glm::vec3(-point->x, -point->y, -point->z));

        apply_matrix(rotation_matrix);
        free(point);
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

    Coordinate *get_center_of_gravity()
    {
        float x_sum, y_sum, z_sum = 0;
        for (int i = 0; i < vertices_count; i += 3)
        {
            x_sum += vertices[i];
            y_sum += vertices[i + 1];
            z_sum += vertices[i + 2];
        }

        return new Coordinate(
            x_sum / (vertices_count / 3),
            y_sum / (vertices_count / 3),
            z_sum / (vertices_count / 3));
    }

    double get_x_center_of_gravity()
    {
        double x_sum = 0;

        for (int i = 0; i < vertices_count; i += 3)
        {
            x_sum += vertices[i];
        }

        return x_sum / (vertices_count / 3);
    }

    void apply_forces()
    {
        for (int i = 0; i < vertices_count; i += 3)
        {
            vertices[i] += x_force;
            vertices[i + 1] += y_force;
        }
        last_collision++;
        last_ground_collision++;
    }

    void update_position()
    {
        if (fixed)
            return;

        check_ground_collision();
        calculate_rotation_force();
        if (last_collision > COLLISION_DELAY)
        {
            calculate_y_force();
            calculate_x_force();
        }

        apply_forces();
    }
};