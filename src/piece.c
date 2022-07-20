#include "piece.h"
#include "shader.h"
#include <string.h>
#include <glad/glad.h>


struct Piece *piece_alloc()
{
    struct Piece *p = malloc(sizeof(struct Piece));

    p->ncubes = 1;
    p->cubes = malloc(sizeof(struct Cube*) * p->ncubes);

    p->cubes[0] = cube_alloc((vec3){ 5.f, 0.f, 0.f }, (vec3){ 0.f, 0.f, 0.f });

    p->nverts = p->ncubes * CUBE_VERTLEN * CUBE_NVERTS;
    p->verts = malloc(sizeof(float) * p->nverts);

    glGenVertexArrays(1, &p->vao);
    glBindVertexArray(p->vao);

    glGenBuffers(1, &p->vb);
    glBindBuffer(GL_ARRAY_BUFFER, p->vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * p->nverts, p->verts, GL_DYNAMIC_DRAW);

    // verts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // norms
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return p;
}


void piece_free(struct Piece *p)
{
    glDeleteBuffers(1, &p->vb);
    glDeleteVertexArrays(1, &p->vao);

    for (size_t i = 0; i < p->ncubes; ++i)
        cube_free(p->cubes[i]);

    free(p->cubes);
    free(p);
}


void piece_move(struct Piece *p, vec3 dir)
{
    for (size_t i = 0; i < p->ncubes; ++i)
        cube_move(p->cubes[i], dir);
}


void piece_render(struct Piece *p, RenderInfo *ri)
{
    size_t index = 0;

    for (size_t i = 0; i < p->ncubes; ++i)
    {
        memcpy(p->verts + index, p->cubes[i]->verts, sizeof(p->cubes[i]->verts));
        index += CUBE_VERTLEN * CUBE_NVERTS;
    }

    glBindBuffer(GL_ARRAY_BUFFER, p->vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * p->nverts, p->verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mat4 model;
    glm_mat4_identity(model);
    shader_mat4(ri->shader, "model", model);

    glBindVertexArray(p->vao);
    glDrawArrays(GL_TRIANGLES, 0, p->ncubes * CUBE_NVERTS);
    glBindVertexArray(0);
}

