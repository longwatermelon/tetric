#ifndef PIECE_H
#define PIECE_H

#include "render.h"
#include "cube.h"

struct Piece
{
    struct Cube **cubes;
    size_t ncubes;

    float *verts;
    size_t nverts;
};

// Piece takes ownership of cubes list
struct Piece *piece_alloc(struct Cube **cubes, size_t ncubes);
void piece_free(struct Piece *p);

void piece_move(struct Piece *p, vec3 dir);
float *piece_verts(struct Piece *p, size_t *len);

#endif

