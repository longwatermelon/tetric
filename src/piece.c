#include "piece.h"
#include "shader.h"
#include <string.h>
#include <glad/glad.h>


struct Piece *piece_alloc(struct Cube **cubes, size_t ncubes)
{
    struct Piece *p = malloc(sizeof(struct Piece));

    p->ncubes = ncubes;
    p->cubes = cubes;

    p->nverts = p->ncubes * CUBE_VERTLEN * CUBE_NVERTS;
    p->verts = malloc(sizeof(float) * p->nverts);

    return p;
}


void piece_free(struct Piece *p)
{
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


float *piece_verts(struct Piece *p)
{
    size_t index = 0;

    for (size_t i = 0; i < p->ncubes; ++i)
    {
        memcpy(p->verts + index, p->cubes[i]->verts, sizeof(p->cubes[i]->verts));
        index += CUBE_VERTLEN * CUBE_NVERTS;
    }

    return p->verts;
}

