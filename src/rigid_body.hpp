#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include "constants.hpp"
#include "geometry/line.hpp"
#include "geometry/coordinate.hpp"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class RigidBody {
  public:
    int id;
    float col_r, col_g, col_b;
    double x_force, y_force, z_force;
    double mass, friction;
    float rotation_force;
    bool fixed;

    float* vertices;
    std::list<Line> collision_area;
    int vertices_count;

    int last_collision = 0;
    int last_ground_collision = 0;

    RigidBody();
    RigidBody(int _id, std::list<float>& _vertices, double _mass, double _friction);
    void set_forces(double _x_force, double _y_force, double _z_force, double _rotation_force);
    void set_fixed(bool _fixed);
    void set_position(double x, double y, double z);
    void set_color(float _col_r, float _col_g, float _col_b);
    void create_circuit_ca();
    bool check_ground_collision();
    void rewind(int n);
    void collide(RigidBody* other_body, double x, double y);
    void calculate_y_force();
    void calculate_x_force();
    void apply_rotation_force();
    void apply_matrix(glm::mat4 model);
    Coordinate* get_center_of_gravity();
    double get_x_center_of_gravity();
    double get_y_center_of_gravity();
    void apply_forces();
    void update_position();
};

#endif