#include "../lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <list>
#include <iostream>
#include <cmath>

#include "constants.hpp"
#include "gl/shaders.hpp"
#include "gl/common.hpp"
#include "geometry/Coordinate.hpp"
#include "geometry/Line.hpp"
#include "geometry/structures.hpp"
#include "RigidBody.hpp"

void redraw(int VAO, RigidBody *r_body, int shader_program)
{
    int color = glGetUniformLocation(shader_program, "color");
    glUniform3f(color, r_body->col_r, r_body->col_g, r_body->col_b);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, r_body->vertices_count / 3);
}

void check_collision(int shader_program, RigidBody *r_body, RigidBody **objects, int objects_size, int iter)
{
    int color = glGetUniformLocation(shader_program, "color");
    glUniform3f(color, 255, 255, 0);

    r_body->create_circuit_ca();
    for (Line &lineA : r_body->collision_area)
    {
        for (int i = iter + 1; i < objects_size; i++)
        {
            RigidBody *objectB = objects[i];
            objectB->create_circuit_ca();
            for (Line &lineB : objectB->collision_area)
            {
                double *intersection = lineA.get_segment_intersect(lineB);
                if (intersection != nullptr)
                {
                    r_body->collide(objectB, intersection[0], intersection[1]);

                    float col_point[3] = {(float)intersection[0], (float)intersection[1], 0};
                    int c_vao = gen_vao(col_point, 3 * sizeof(float));
                    glBindVertexArray(c_vao);
                    glDrawArrays(GL_POINTS, 0, 1);
                    free(intersection);
                }
            }
        }
    }
}

int main()
{
    int obj_cnt = 4;
    RigidBody *r_body = new RigidBody(
        0,
        triangle_struct1,
        0.3, //mass
        0.6, //friction
        0.5  //bounciness
    );
    r_body->set_color(0.8f, 0.1f, 0.1f);
    r_body->set_positon(0.1, 0.1, 0);
    r_body->set_forces(-0.0025, 0.001, 0, 0.3);

    RigidBody *r_body2 = new RigidBody(
        1,
        triangle_struct2,
        0.8, //mass
        0.4, //friction
        0.5  //bounciness
    );
    r_body2->set_color(0.1f, 0.1f, 0.8f);
    r_body2->set_positon(-0.3, 0.8, 0);
    r_body2->set_forces(0.005, -0.005, 0, 0.4);

    RigidBody *r_body3 = new RigidBody(
        2,
        triangle_struct2,
        1,    //mass
        0.05, //friction
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

    RigidBody *r_bodies[obj_cnt] = {r_body, r_body2, r_body3, r_body4};

    int vaos[obj_cnt];
    GLFWwindow *window = create_window();
    int shader_program = create_shader_program();

    int previous_time = glfwGetTime();
    int frame_count = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        for (int i = 0; i < obj_cnt; i++)
        {
            RigidBody *object = r_bodies[i];
            object->update_position();

            vaos[i] = gen_vao(object->vertices, object->vertices_count * sizeof(float));
            redraw(vaos[i], object, shader_program);
        }
        for (int i = 0; i < obj_cnt; i++)
        {
            check_collision(shader_program, r_bodies[i], r_bodies, obj_cnt, i);
        }
        frame_count++;
        int current_time = glfwGetTime();
        if (current_time - previous_time >= 1)
        {
            printf("fps: %d \n", frame_count);
            previous_time = current_time;
            frame_count = 0;
        }

        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
