#ifndef BOARD_H
#define BOARD_H

#include "piece.h"

struct Board
{
    char *layout;

    struct Piece **pieces;
    size_t npieces;
};

struct Board *board_alloc();
void board_free(struct Board *b);

void board_render(struct Board *b);

#endif

