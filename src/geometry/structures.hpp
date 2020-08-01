#include <list>

std::list<float> triangle_struct1 = {
    0.0f, 0.0f, 0.0f,
    0.2f, 0.0f, 0.0f,
    0.1f, 0.2f, 0.0f};

std::list<float> triangle_struct2 = {
    0.0f, 0.0f, 0.0f,
    0.2f, 0.0f, 0.0f,
    0.1f, 0.3f, 0.0f};

std::list<float> square_struct1 = {
    0.0f, 0.0f, 0.0f,
    0.2f, 0.0f, 0.0f,
    0.3f, 0.2f, 0.0f,
    0.1f, 0.4f, 0.0f};

void create_rigid_bodies(RigidBody **r_bodies, int obj_cnt)
{
    RigidBody *r_body = new RigidBody(
        0,
        triangle_struct1,
        0.3, //mass
        0.99, //friction
        0.5  //bounciness
    );
    r_body->set_color(0.8f, 0.1f, 0.1f);
    r_body->set_positon(0.1, 0.1, 0);
    r_body->set_forces(-0.0025, 0.001, 0, 0.3);

    RigidBody *r_body2 = new RigidBody(
        1,
        triangle_struct2,
        0.8, //mass
        0.99, //friction
        0.5  //bounciness
    );
    r_body2->set_color(0.1f, 0.1f, 0.8f);
    r_body2->set_positon(-0.3, 0.8, 0);
    r_body2->set_forces(0.005, -0.005, 0, 0.4);

    RigidBody *r_body3 = new RigidBody(
        2,
        triangle_struct2,
        1,    //mass
        0.5, //friction
        0.6   //bounciness
    );
    r_body3->set_color(0.3f, 0.3f, 0.1f);
    r_body3->set_positon(-0.1, 0.5, 0);
    r_body3->set_forces(0, 0.002, 0, 0.05);
    r_body3->set_fixed(true);

    RigidBody *r_body4 = new RigidBody(
        3,
        square_struct1,
        0.3, //mass
        0.4, //friction
        0.7  //bounciness
    );
    r_body4->set_positon(-0.1, -0.3, 0);
    r_body4->set_forces(-0.002, -0.005, 0, 0);
    r_body4->set_color(0.8f, 0.3f, 0.8f);

    RigidBody *lr_bodies[obj_cnt] = {r_body, r_body2, r_body3, r_body4};
    for (int i = 0; i < obj_cnt; i++)
        r_bodies[i] = lr_bodies[i];
}