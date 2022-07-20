#include "board.h"
#include "shader.h"
#include <string.h>
#include <glad/glad.h>


struct Board *board_alloc()
{
    struct Board *b = malloc(sizeof(struct Board));
    b->layout = calloc(10 * 20 + 1, sizeof(char));

    for (size_t i = 0; i < 20; ++i)
        strcat(b->layout, "..........");

    b->pieces = 0;
    b->npieces = 0;
    b->verts = 0;
    b->nverts = 0;

    board_make_borders(b);

    glGenVertexArrays(1, &b->vao);
    glBindVertexArray(b->vao);

    glGenBuffers(1, &b->vb);
    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * b->nverts, b->verts, GL_DYNAMIC_DRAW);

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

    return b;
}


void board_free(struct Board *b)
{
    free(b->layout);

    for (size_t i = 0; i < b->npieces; ++i)
        piece_free(b->pieces[i]);

    free(b->pieces);

    glDeleteVertexArrays(1, &b->vao);
    glDeleteBuffers(1, &b->vb);
    free(b);
}


void board_render(struct Board *b, RenderInfo *ri)
{
    board_fill_verts(b);

    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * b->nverts, b->verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mat4 model;
    glm_mat4_identity(model);
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

/* void piece_render(struct Piece *p, RenderInfo *ri) */
/* { */
/*     size_t index = 0; */

/*     for (size_t i = 0; i < p->ncubes; ++i) */
/*     { */
/*         memcpy(p->verts + index, p->cubes[i]->verts, sizeof(p->cubes[i]->verts)); */
/*         index += CUBE_VERTLEN * CUBE_NVERTS; */
/*     } */

/* } */

void board_add_piece(struct Board *b, struct Piece *p)
{
    b->pieces = realloc(b->pieces, sizeof(struct Piece*) * ++b->npieces);
    b->pieces[b->npieces - 1] = p;

    b->nverts += p->nverts;
    b->verts = realloc(b->verts, sizeof(float) * b->nverts);
}


void board_make_borders(struct Board *b)
{
    size_t ncubes = 8 + 20 * 2;
    struct Cube **cubes = malloc(sizeof(struct Cube*) * ncubes);
    size_t index = 0;

    for (size_t i = 0; i < 20; ++i)
    {
        b->layout[i * 10] = '#';
        cubes[index++] = cube_alloc((vec3){ 10.f, i, 0.f }, (vec3){ 1.f, 1.f, 1.f });

        b->layout[i * 10 + 9] = '#';
        cubes[index++] = cube_alloc((vec3){ 10.f, i, 9.f }, (vec3){ 1.f, 1.f, 1.f });
    }

    for (size_t i = 0; i < 9; ++i)
    {
        b->layout[((10 * 20 - 1) - 10) + (i + 1)] = '#';
        cubes[index++] = cube_alloc((vec3){ 10.f, 0.f, i + 1 }, (vec3){ 1.f, 1.f, 1.f });
    }

    /* for (size_t i = 0; i < 10 * 20; ++i) */
    /* { */
    /*     if (i % 10 == 0) putchar('\n'); */
    /*     putchar(b->layout[i]); */
    /* } */
    /* putchar('\n'); */

    /* printf("%s\n", b->layout); */

    struct Piece *p = piece_alloc(cubes, ncubes);
    board_add_piece(b, p);
}

