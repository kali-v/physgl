#include <iostream>

#include "../lib/glad/glad.h"

const char *vert_shader_src =
    "#version 440 core\n"
    "layout (location = 0) in vec3 def_pos;\n"
    "uniform vec3 cur_pos;"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4( \n"
    "        def_pos.x + cur_pos.x, \n"
    "        def_pos.y + cur_pos.y, \n"
    "        def_pos.z + cur_pos.z, \n"
    "        1.0); \n"
    "}\0";

const char *frag_shader_src =
    "#version 440 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(color.x,color.y,color.z,1.0f);\n"
    "}\n\0";

int create_shader_program()
{
    unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vert_shader_src, NULL);
    glCompileShader(vertex_shader_id);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &frag_shader_src, NULL);
    glCompileShader(fragment_shader);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader_id);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader);

    return shader_program;
}