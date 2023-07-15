#include "../lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <functional>
#include <iostream>
#include <list>

#include "constants.hpp"
#include "geometry/coordinate.hpp"
#include "geometry/line.hpp"
#include "gl/common.hpp"
#include "gl/shaders.hpp"

#include "rigid_body.hpp"

#include "geometry/common.hpp"
#include "geometry/structures.hpp"

std::vector<RigidBody*> rbodies;

void redraw(int VAO, RigidBody* r_body, int shader_program) {
    int color = glGetUniformLocation(shader_program, "color");
    glUniform3f(color, r_body->col_r, r_body->col_g, r_body->col_b);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, r_body->vertices_count / 3);
}

bool control_act[7];

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool con_action = false;
    if (action == GLFW_PRESS) con_action = true;
    else if (action != GLFW_RELEASE) return;

    if (key == GLFW_KEY_LEFT) control_act[0] = con_action;
    else if (key == GLFW_KEY_RIGHT) control_act[1] = con_action;
    else if (key == GLFW_KEY_UP) control_act[2] = con_action;
    else if (key == GLFW_KEY_DOWN) control_act[3] = con_action;
    else if (key == GLFW_KEY_E) control_act[4] = con_action;
    else if (key == GLFW_KEY_Q) control_act[5] = con_action;
}

void update_control() {
    if (control_act[0]) rbodies[0]->x_force -= CONTROL_FORCE / 2;
    if (control_act[1]) rbodies[0]->x_force += CONTROL_FORCE / 2;
    if (control_act[2]) rbodies[0]->y_force += CONTROL_FORCE;
    if (control_act[3]) rbodies[0]->y_force -= CONTROL_FORCE;
    if (control_act[4]) rbodies[0]->rotation_force -= CONTROL_FORCE * 50;
    if (control_act[5]) rbodies[0]->rotation_force += CONTROL_FORCE * 50;
}

int main() {
    rbodies = create_rigid_bodies();
    int obj_cnt = rbodies.size();

    int vaos[obj_cnt];
    GLFWwindow* window = create_window();
    int shader_program = create_shader_program();

    glfwSetKeyCallback(window, key_callback);

    int previous_time = glfwGetTime();
    int frame_count = 0;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        for (int i = 0; i < obj_cnt; i++) {
            RigidBody* object = rbodies[i];
            object->update_position();

            vaos[i] = gen_vao(object->vertices, object->vertices_count * sizeof(float));
            redraw(vaos[i], object, shader_program);
        }
        for (int i = 0; i < obj_cnt; i++) {
            check_collision(shader_program, rbodies[i], rbodies, obj_cnt, i);
        }

        frame_count++;
        int current_time = glfwGetTime();
        if (current_time - previous_time >= 1) {
            printf("fps: %d \n", frame_count);
            previous_time = current_time;
            frame_count = 0;
        }
        update_control();
        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
