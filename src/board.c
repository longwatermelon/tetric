#include "board.h"
#include "shader.h"
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct Board *board_alloc()
{
    struct Board *b = malloc(sizeof(struct Board));
    b->layout = calloc(10 * 20 + 1, sizeof(char));

    for (size_t i = 0; i < 20; ++i)
        strcat(b->layout, "..........");

    b->pieces = 0;
    b->npieces = 0;

    b->active = 0;
    b->last_moved = 0.f;

    b->verts = 0;
    b->nverts = 0;

    glGenVertexArrays(1, &b->vao);
    glBindVertexArray(b->vao);

    glGenBuffers(1, &b->vb);
    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

    // verts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, CUBE_VERTLEN * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // norms
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, CUBE_VERTLEN * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // colors
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, CUBE_VERTLEN * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    board_make_borders(b);

    return b;
}


void board_free(struct Board *b)
{
    for (size_t i = 0; i < b->npieces; ++i)
        piece_free(b->pieces[i]);

    free(b->pieces);

    glDeleteVertexArrays(1, &b->vao);
    glDeleteBuffers(1, &b->vb);

    free(b->layout);
    free(b);
}


void board_update(struct Board *b)
{
    if (!b->active)
    {
        struct Cube **cubes = malloc(sizeof(struct Cube*));
        cubes[0] = cube_alloc((vec3){ 0.f, 19.f, 5.f }, (vec3){ 1.f, 0.f, 0.f });

        b->active = piece_alloc(cubes, 1);
        board_add_piece(b, b->active);

        b->last_moved = glfwGetTime();
    }

    if (glfwGetTime() - b->last_moved > .5f)
    {
        b->last_moved = glfwGetTime();
        piece_move(b->active, (vec3){ 0.f, -1.f, 0.f });

        if (board_check_collision(b))
        {
            piece_move(b->active, (vec3){ 0.f, 1.f, 0.f });
            board_place_active(b);
            b->active = 0;
        }
    }
}


void board_render(struct Board *b, RenderInfo *ri)
{
    board_fill_verts(b);

    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * b->nverts, b->verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){ 30.f, -10.f, -4.5f });
    shader_mat4(ri->shader, "model", model);

    glBindVertexArray(b->vao);
    glDrawArrays(GL_TRIANGLES, 0, b->nverts / CUBE_VERTLEN);
    glBindVertexArray(0);
}


void board_fill_verts(struct Board *b)
{
    size_t index = 0;

    for (size_t i = 0; i < b->npieces; ++i)
    {
        size_t nverts = b->pieces[i]->nverts;
        float *pverts = piece_verts(b->pieces[i]);

        memcpy(b->verts + index, pverts, sizeof(float) * nverts);
        index += nverts;
    }
}


bool board_check_collision(struct Board *b)
{
    for (size_t i = 0; i < b->active->ncubes; ++i)
    {
        struct Cube *c = b->active->cubes[i];
        int iz = c->pos[2];
        int iy = 19.f - c->pos[1];

        if (b->layout[iy * 10 + iz] == '#')
            return true;
    }

    return false;
}


void board_place_active(struct Board *b)
{
    for (size_t i = 0; i < b->active->ncubes; ++i)
    {
        struct Cube *c = b->active->cubes[i];
        int iz = c->pos[2];
        int iy = 19.f - c->pos[1];

        b->layout[iy * 10 + iz] = '#';
    }
}


void board_move_active(struct Board *b, vec3 dir)
{
    if (!b->active)
        return;

    piece_move(b->active, dir);

    if (board_check_collision(b))
    {
        vec3 back;
        glm_vec3_negate_to(dir, back);
        piece_move(b->active, back);
    }
}


void board_add_piece(struct Board *b, struct Piece *p)
{
    b->pieces = realloc(b->pieces, sizeof(struct Piece*) * ++b->npieces);
    b->pieces[b->npieces - 1] = p;

    b->nverts += p->nverts;
    b->verts = realloc(b->verts, sizeof(float) * b->nverts);

    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * b->nverts, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void board_make_borders(struct Board *b)
{
    size_t ncubes = 8 + 20 * 2;
    struct Cube **cubes = malloc(sizeof(struct Cube*) * ncubes);
    size_t index = 0;

    for (size_t i = 0; i < 20; ++i)
    {
        b->layout[i * 10] = '#';
        cubes[index++] = cube_alloc((vec3){ 0.f, i, 0.f }, (vec3){ 1.f, 1.f, 1.f });

        b->layout[i * 10 + 9] = '#';
        cubes[index++] = cube_alloc((vec3){ 0.f, i, 9.f }, (vec3){ 1.f, 1.f, 1.f });
    }

    for (size_t i = 0; i < 8; ++i)
    {
        b->layout[((10 * 20 - 1) - 10) + (i + 2)] = '#';
        cubes[index++] = cube_alloc((vec3){ 0.f, 0.f, i + 1 }, (vec3){ 1.f, 1.f, 1.f });
    }

    /* for (size_t i = 0; i < 10 * 20; ++i) */
    /* { */
    /*     if (i % 10 == 0) putchar('\n'); */
    /*     putchar(b->layout[i]); */
    /* } */

    /* putchar('\n'); */

    struct Piece *p = piece_alloc(cubes, ncubes);
    board_add_piece(b, p);
}

