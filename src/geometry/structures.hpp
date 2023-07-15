#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <list>
#include <vector>

std::list<float> triangle_struct1 = {0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.1f, 0.2f, 0.0f};

std::list<float> triangle_struct2 = {0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.1f, 0.3f, 0.0f};

std::list<float> custom_struct1 = {0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.3f, 0.2f, 0.0f, 0.1f, 0.4f, 0.0f};

std::list<float> custom_struct2 = {
    0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.15f, 0.2f, 0.0f, 0.3f, 0.15f, 0.0f, 0.26f, 0.3f, 0.0f, 0.05f, 0.27f, 0.0f,
};

std::vector<RigidBody*> create_rigid_bodies() {
    RigidBody* r_body = new RigidBody(0, triangle_struct1, 0.3, 0.2);
    r_body->set_color(0.8f, 0.1f, 0.1f);
    r_body->set_position(0.1, 0.1, 0);
    r_body->set_forces(-0.0025, 0.001, 0, 0.3);

    RigidBody* r_body2 = new RigidBody(1, custom_struct1, 0.7, 0.4);
    r_body2->set_color(0.1f, 0.1f, 0.8f);
    r_body2->set_position(0.3, 0.8, 0);
    r_body2->set_forces(-0.005, 0.005, 0, 0.4);

    RigidBody* r_body3 = new RigidBody(2, custom_struct2, 0.3, 0.07);
    r_body3->set_position(-0.1, -0.3, 0);
    r_body3->set_forces(-0.002, -0.005, 0, 0);
    r_body3->set_color(0.8f, 0.3f, 0.8f);

    RigidBody* r_body4 = new RigidBody(3, triangle_struct2, 2, 0.1);
    r_body4->set_color(0.3f, 0.3f, 0.1f);
    r_body4->set_position(-0.1, 0.5, 0);
    r_body4->set_forces(0, 0.002, 0, 0.05);
    r_body4->set_fixed(true);

    return {r_body, r_body2, r_body3, r_body4};
}

#endif