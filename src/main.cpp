#include "../lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <list>
#include <unordered_map>

#include "constants.hpp"
#include "variables.hpp"

#include "geometry/coordinate.hpp"
#include "geometry/line.hpp"
#include "gl/common.hpp"
#include "gl/shaders.hpp"

#include "geometry/common.hpp"
#include "rigid_body.hpp"
#include "structures.hpp"

std::vector<RigidBody*> rbodies;

bool control_act[12];

std::vector<int> keys_control = {GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_E, GLFW_KEY_Q,
                                 GLFW_KEY_A,    GLFW_KEY_Z,     GLFW_KEY_S,  GLFW_KEY_X,    GLFW_KEY_D, GLFW_KEY_C};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    for (int i = 0; i < keys_control.size(); i++) {
        if (key == keys_control[i]) control_act[i] = (action != GLFW_RELEASE);
    }
}

void update_control() {
    if (control_act[0]) rbodies[0]->x_force -= control_force / 2;
    if (control_act[1]) rbodies[0]->x_force += control_force / 2;
    if (control_act[2]) rbodies[0]->y_force += control_force;
    if (control_act[3]) rbodies[0]->y_force -= control_force;
    if (control_act[4]) rbodies[0]->rotation_force -= control_force * 50;
    if (control_act[5]) rbodies[0]->rotation_force += control_force * 50;
    if (control_act[6]) control_force *= 1.01;
    if (control_act[7]) control_force /= 1.01;
    if (control_act[8]) air_force *= 1.01;
    if (control_act[9]) air_force /= 1.01;
    if (control_act[10]) g_force *= 1.01;
    if (control_act[11]) g_force /= 1.01;
}

void redraw(int VAO, RigidBody* r_body, int shader_program) {}

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
            glUniform3f(glGetUniformLocation(shader_program, "color"), object->col_r, object->col_g, object->col_b);
            glBindVertexArray(vaos[i]);
            glDrawArrays(GL_LINE_LOOP, 0, object->vertices_count / 3);
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
