void check_collision(int shader_program, RigidBody *r_body, RigidBody **objects, int objects_size, int iter)
{
    int color = glGetUniformLocation(shader_program, "color");
    glUniform3f(color, 255, 255, 0);

    r_body->create_circuit_ca();
    for (Line &lineA : r_body->collision_area)
    {
        for (int i = iter + 1; i < objects_size; i++)
        {
            RigidBody *other_body = objects[i];
            other_body->create_circuit_ca();
            for (Line &lineB : other_body->collision_area)
            {
                double *intersection = lineA.get_segment_intersect(lineB);
                if (intersection != nullptr)
                {
                    r_body->collide(other_body, intersection[0], intersection[1]);

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