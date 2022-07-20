#ifndef PIECE_H
#define PIECE_H

#include "render.h"
#include "cube.h"

struct Piece
{
    struct Cube **cubes;
    size_t ncubes;

    unsigned int vao, vb;

    float *verts;
    size_t nverts;
};

struct Piece *piece_alloc();
void piece_free(struct Piece *p);

void piece_move(struct Piece *p, vec3 dir);
void piece_render(struct Piece *p, RenderInfo *ri);

#endif

