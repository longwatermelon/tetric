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

            board_clear_full_lines(b);
        }
    }
}


void board_render(struct Board *b, RenderInfo *ri)
{
    size_t len = board_fill_verts(b);

    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * b->nverts, b->verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){ 30.f, -10.f, -4.5f });
    shader_mat4(ri->shader, "model", model);

    glBindVertexArray(b->vao);
    glDrawArrays(GL_TRIANGLES, 0, len / CUBE_VERTLEN);
    glBindVertexArray(0);
}


size_t board_fill_verts(struct Board *b)
{
    size_t index = 0;

    for (size_t i = 0; i < b->npieces; ++i)
    {
        size_t len;
        float *pverts = piece_verts(b->pieces[i], &len);

        memcpy(b->verts + index, pverts, sizeof(float) * len);
        index += len;
    }

    return index;
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


void board_clear_full_lines(struct Board *b)
{
    int cleared = 0;
    int lowest = 20;

    for (size_t i = 0; i < 19; ++i)
    {
        size_t begin = i * 10;
        bool full = true;

        for (size_t z = begin + 1; z <= begin + 8; ++z)
        {
            if (b->layout[z] != '#')
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            ++cleared;
            int iy = i;

            for (size_t z = begin + 1; z <= begin + 8; ++z)
            {
                int iz = z - begin;

                for (size_t i = 0; i < b->npieces; ++i)
                {
                    for (size_t j = 0; j < b->pieces[i]->ncubes; ++j)
                    {
                        struct Cube *c = b->pieces[i]->cubes[j];
                        int ciy = 19.f - c->pos[1];
                        int ciz = c->pos[2];

                        if (ciy == iy && ciz == iz)
                            b->pieces[i]->cubes[j]->render = false;
                    }
                }

                b->layout[z] = '.';
            }

            if (iy < lowest)
                lowest = iy;
        }
    }

    if (cleared)
    {
        // Start from i = 1 to ignore border pieces
        for (size_t i = 1; i < b->npieces; ++i)
            piece_move(b->pieces[i], (vec3){ 0.f, -1.f * cleared, 0.f });

        for (int i = lowest - 1; i >= 0; --i)
        {
            for (size_t z = 1; z <= 8; ++z)
            {
                if (b->layout[i * 10 + z] == '#')
                {
                    b->layout[i * 10 + z] = '.';
                    // @ indicates newly moved piece so it doesn't get
                    // cleared by another piece moving out of that position
                    b->layout[i * 10 + 10 + z] = '@';
                }
            }
        }

        for (size_t i = 0; i < strlen(b->layout); ++i)
        {
            if (b->layout[i] == '@')
                b->layout[i] = '#';
        }
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

