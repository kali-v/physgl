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
#include "TObject.hpp"

void redraw(int VAO, TObject *tobject, int shader_program)
{
    int color = glGetUniformLocation(shader_program, "color");
    glUniform3f(color, tobject->col_r, tobject->col_g, tobject->col_b);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, tobject->vertices_count / 3);
}

void check_collision(int shader_program, TObject *tobject, TObject *objects, int objects_size, int iter)
{
    int color = glGetUniformLocation(shader_program, "color");
    glUniform3f(color, 255, 255, 0);
    tobject->create_circuit_ca();

    for (Line &lineA : tobject->collision_area)
    {
        for (int i = iter + 1; i < objects_size; i++)
        {
            TObject objectB = (objects[i]);
            objectB.create_circuit_ca();
            for (Line &lineB : objectB.collision_area)
            {
                double *intersection = lineA.get_segment_intersect(lineB);
                if (intersection != nullptr)
                {
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
    std::list<float> start_obj_list = {
        0.0f, 0.0f, 0.0f,
        0.2f, 0.0f, 0.0f,
        0.1f, 0.2f, 0.0f};

    std::list<float> start_obj_list2 = {
        0.0f, 0.0f, 0.0f,
        0.2f, 0.0f, 0.0f,
        0.1f, 0.3f, 0.0f};

    std::list<float> start_obj_square = {
        0.0f, 0.0f, 0.0f,
        0.2f, 0.0f, 0.0f,
        0.3f, 0.2f, 0.0f,
        0.0f, 0.4f, 0.0f};

    int obj_cnt = 4;
    TObject tobject = TObject(
        0,
        start_obj_list,
        1.0, //weight
        0.6, //friction
        0.6, //bounciness
        0.8f, 0.1f, 0.1f);
    TObject tobject2 = TObject(
        1,
        start_obj_list2,
        0.5, //weight
        0.4, //friction
        0.4, //bounciness
        0.1f, 0.1f, 0.8f);
    TObject tobject3 = TObject(
        2,
        start_obj_list2,
        1,   //weight
        0.4, //friction
        0.1, //bounciness
        0.3f, 0.3f, 0.1f);
    TObject tobject4 = TObject(
        3,
        start_obj_square,
        0.2, //weight
        0.4, //friction
        0.5, //bounciness
        0.8f, 0.3f, 0.8f);

    TObject tobjects[obj_cnt] = {tobject, tobject2, tobject3, tobject4};

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
            TObject *object = &tobjects[i];
            object->update_position();

            vaos[i] = gen_vao(object->vertices, object->vertices_count * sizeof(float));
            redraw(vaos[i], object, shader_program);
        }
        for (int i = 0; i < obj_cnt; i++)
        {
            check_collision(shader_program, &tobjects[i], tobjects, obj_cnt, i);
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
