#include <cmath>
#include <list>

#include "constants.hpp"
#include "geometry/coordinate.hpp"
#include "rigid_body.hpp"
#include "variables.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

RigidBody::RigidBody(int _id, std::list<float>& _vertices, double _mass, double _friction) {
    id = _id;

    vertices_count = _vertices.size();
    vertices = (float*)malloc(vertices_count * sizeof(float));
    std::copy(_vertices.begin(), _vertices.end(), vertices);

    // physical properties
    friction = _friction;
    mass = _mass;

    fixed = false;

    x_force = 0;
    y_force = 0;
    z_force = 0;
    rotation_force = 0;

    col_r = 127;
    col_g = 127;
    col_b = 127;
}

void RigidBody::set_forces(double _x_force, double _y_force, double _z_force, double _rotation_force) {
    x_force = _x_force;
    y_force = _y_force;
    z_force = _z_force;
    rotation_force = _rotation_force;
}

void RigidBody::set_fixed(bool _fixed) { fixed = _fixed; }

void RigidBody::set_position(double x, double y, double z) {
    for (int i = 0; i < vertices_count; i += 3) {
        vertices[i] += x;
        vertices[i + 1] += y;
        vertices[i + 2] += z;
    }
}

void RigidBody::set_color(float _col_r, float _col_g, float _col_b) {
    col_r = _col_r;
    col_g = _col_g;
    col_b = _col_b;
}

void RigidBody::create_circuit_ca() {
    collision_area.clear();
    for (int i = 0; i < vertices_count; i += 3) {
        Coordinate* a = new Coordinate(vertices[i], vertices[i + 1], vertices[i + 2]);
        Coordinate* b = (i + 3 == vertices_count) ? new Coordinate(vertices[0], vertices[1], vertices[2])
                                                  : new Coordinate(vertices[i + 3], vertices[i + 4], vertices[i + 5]);

        Line* line = new Line(*a, *b);
        delete a;
        delete b;
        collision_area.push_back(*line);
    }
}

bool RigidBody::check_ground_collision() {
    bool left_touch = false;
    bool right_touch = false;
    for (int i = 1; i < vertices_count; i += 3) {
        float cy = vertices[i];
        if (cy < -1) {
            float diff_x = get_x_center_of_gravity() - vertices[i - 1];
            if (diff_x > EQ_TRESH) {
                left_touch = true;
                rotation_force -= (.1f + diff_x / rot_factor) / mass;
            } else if (diff_x < -EQ_TRESH) {
                right_touch = true;
                rotation_force += (.1f + diff_x / rot_factor) / mass;
            }

            for (int j = 1; j <= vertices_count; j += 3) {
                vertices[j] -= 1 + cy;
            }

            x_force *= (1 - friction);
            if (fabs(y_force) > EQ_TRESH) y_force = -(.3 * y_force) / (mass * 1.5f);
        }
    }

    if (left_touch || right_touch) {
        if (left_touch && right_touch || fabs(rotation_force) < EQ_TRESH) rotation_force = 0;
        last_ground_collision = 0;
        return true;
    }
    return false;
}

void RigidBody::rewind(int n) {
    this->x_force = -this->x_force;
    this->y_force = -this->y_force;
    this->rotation_force = -this->rotation_force;
    for (int i = 0; i < n; i++) {
        apply_forces();
        apply_rotation_force();
    }
    this->x_force = -this->x_force;
    this->y_force = -this->y_force;
    this->rotation_force = -this->rotation_force;
}

void RigidBody::collide(RigidBody* other_body, double x, double y) {
    if (last_collision > COLLISION_DELAY) {
        float diff_x = get_x_center_of_gravity() - x;
        float diff_y = get_y_center_of_gravity() - y;

        double force_diff = fabs(x_force - other_body->x_force) + fabs(y_force - other_body->y_force);
        rotation_force -= (rot_factor * (get_x_center_of_gravity() - x) / mass) / (rotation_force + 1);
        other_body->rotation_force -=
            (rot_factor * (other_body->get_x_center_of_gravity() - x) / mass) / (other_body->rotation_force + 1);
        // other_body->rotation_force -=
        rot_factor*(other_body->get_x_center_of_gravity() - x) / mass / (other_body->rotation_force + 1);

        double sw = mass + other_body->mass;

        double xa = (mass - other_body->mass) / sw;
        double xb = (2 * other_body->mass) / sw;

        double tmp_x_force = x_force;
        double tmp_y_force = y_force;

        x_force = ((xa * x_force) + (xb * other_body->x_force)) + diff_x * x_factor;
        y_force = ((xa * y_force) + (xb * other_body->y_force)) - diff_y * y_factor;

        xa = (2 * mass) / sw;
        xb = (other_body->mass - mass) / sw;

        other_body->x_force = ((xa * tmp_x_force) + (xb * other_body->x_force)) - diff_x * x_factor;
        other_body->y_force = ((xa * tmp_y_force) + (xb * other_body->y_force)) + diff_y * y_factor;

        last_collision = 0;
    }
}

void RigidBody::calculate_y_force() { y_force -= g_force * sim_scale; }

void RigidBody::calculate_x_force() {
    if (fabs(x_force) > EQ_TRESH) {
        x_force += sim_scale * ((x_force > 0) ? -air_force : air_force);
        if ((x_force > 0) != x_force > 0) x_force = 0;
    } else {
        x_force = 0;
    }
}

void RigidBody::apply_rotation_force() {
    rotation_force += sim_scale * ((rotation_force > 0) ? -air_force : air_force);
    if (fabs(rotation_force) <= EQ_TRESH) rotation_force = 0;

    Coordinate* point = get_center_of_gravity();

    glm::mat4 rotation_matrix = glm::mat4(1.0f);
    rotation_matrix = glm::translate(rotation_matrix, glm::vec3(point->x, point->y, point->z));
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation_force), glm::vec3(0.0f, 0.0f, 1.0f));
    rotation_matrix = glm::translate(rotation_matrix, glm::vec3(-point->x, -point->y, -point->z));
    free(point);

    apply_matrix(rotation_matrix);
}

void RigidBody::apply_matrix(glm::mat4 model) {
    for (int i = 0; i < vertices_count; i += 3) {
        glm::vec4 cvec = glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1);
        cvec = model * cvec;
        vertices[i] = cvec.x;
        vertices[i + 1] = cvec.y;
        vertices[i + 2] = cvec.z;
    }
}

Coordinate* RigidBody::get_center_of_gravity() {
    float x_sum = 0;
    float y_sum = 0;
    float z_sum = 0;
    for (int i = 0; i < vertices_count; i += 3) {
        x_sum += vertices[i];
        y_sum += vertices[i + 1];
        z_sum += vertices[i + 2];
    }

    return new Coordinate(x_sum / (vertices_count / 3), y_sum / (vertices_count / 3), z_sum / (vertices_count / 3));
}

double RigidBody::get_x_center_of_gravity() {
    double x_sum = 0;
    for (int i = 0; i < vertices_count; i += 3) {
        x_sum += vertices[i];
    }
    return x_sum / (vertices_count / 3);
}

double RigidBody::get_y_center_of_gravity() {
    double y_sum = 0;
    for (int i = 1; i < vertices_count; i += 3) {
        y_sum += vertices[i];
    }
    return y_sum / (vertices_count / 3);
}

void RigidBody::apply_forces() {
    for (int i = 0; i < vertices_count; i += 3) {
        vertices[i] += x_force;
        vertices[i + 1] += y_force;
    }
    last_collision++;
    last_ground_collision++;
}

void RigidBody::update_position() {
    if (fixed) return;

    check_ground_collision();
    apply_rotation_force();

    calculate_y_force();
    calculate_x_force();
    apply_forces();
}