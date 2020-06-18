#include "../lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <list>
#include <iostream>
#include <cmath>

#include "shaders.hpp"
#include "constants.hpp"
#include "geometry/Coordinate.hpp"
#include "geometry/Line.hpp"
#include "TObject.hpp"

GLFWwindow *create_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    if (DEBUG_MODE)
    {
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1920, 1200, "M", glfwGetPrimaryMonitor(), nullptr);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return NULL;

    return window;
}

int gen_vao(float object[], size_t object_size)
{
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, object_size, object, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    return VAO;
}

void redraw(int VAO, TObject *tobject, int shader_program)
{
    tobject->update_position();
    int new_pos = glGetUniformLocation(shader_program, "cur_pos");
    int color = glGetUniformLocation(shader_program, "color");
    glUniform3f(new_pos, tobject->x, tobject->y, tobject->z);
    glUniform3f(color, tobject->col_r, tobject->col_g, tobject->col_b);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, tobject->vertices_size / sizeof(float));
}

void check_collision(int VAO, int shader_program, TObject *tobject, TObject *objects, int objects_size)
{
    int new_pos = glGetUniformLocation(shader_program, "cur_pos");
    int color = glGetUniformLocation(shader_program, "color");
    for (Coordinate &point : tobject->collision_area)
    {
        Coordinate *cur_pos = new Coordinate(
            point.x + (tobject->x),
            point.y + (tobject->y),
            point.z + (tobject->z));
        for (int i = 0; i < objects_size; i++)
        {
            if (objects[i].id != tobject->id)
            {
                for (Coordinate &tpoint : (objects[i]).collision_area)
                {
                    Coordinate *cur_tpos = new Coordinate(
                        tpoint.x + ((objects[i]).x),
                        tpoint.y + ((objects[i]).y),
                        tpoint.z + ((objects[i]).z));
                    if (cur_pos->is_near(*cur_tpos))
                    {
                        glUniform3f(
                            new_pos,
                            cur_tpos->x,
                            cur_tpos->y,
                            cur_tpos->z);
                        glUniform3f(color, 255, 255, 0);
                        glBindVertexArray(VAO);
                        glDrawArrays(GL_POINTS, 0, 1);
                        break;
                    }
                    free(cur_tpos);
                }
            }
        }
        free(cur_pos);
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

    int obj_cnt = 3;
    TObject tobject = TObject(
        0,
        start_obj_list,
        0, 0, 0,
        1.5, //weight
        0.6, //friction
        0.6, //bounciness
        0.8f, 0.1f, 0.1f);
    TObject tobject2 = TObject(
        1,
        start_obj_list2,
        -0.2, 0, 0,
        2,   //weight
        0.5, //friction
        0.5, //bounciness
        0.1f, 0.1f, 0.8f);
    TObject tobject3 = TObject(
        2,
        start_obj_list2,
        -0.4, -0.2, 0,
        2.1, //weight
        0.4, //friction
        0.7, //bounciness
        0.9f, 0.9f, 0.9f);

    TObject tobjects[obj_cnt] = {
        tobject, tobject2, tobject3};

    int vaos[obj_cnt];
    GLFWwindow *window = create_window();
    if (window == NULL)
        return -1;

    int shader_program = create_shader_program();

    for (int i = 0; i < obj_cnt; i++)
    {
        vaos[i] = gen_vao(tobjects[i].vertices, tobjects[i].vertices_size);
    }

    int previous_time = glfwGetTime();
    int frame_count = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);

        for (int i = 0; i < obj_cnt; i++)
        {
            redraw(vaos[i], &tobjects[i], shader_program);
            check_collision(vaos[i], shader_program, &tobjects[i], tobjects, obj_cnt);
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