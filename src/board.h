#ifndef BOARD_H
#define BOARD_H

#include "render.h"
#include "piece.h"

struct Board
{
    char *layout;

    struct Piece **pieces;
    size_t npieces;

    struct Piece *active;
    struct Piece *hold;
    float last_moved;

    float *verts;
    size_t nverts;

    unsigned int vao, vb;

    bool can_hold;

    int next_piece;
};

struct Board *board_alloc();
void board_free(struct Board *b);

void board_update(struct Board *b);

void board_render(struct Board *b, RenderInfo *ri);
size_t board_fill_verts(struct Board *b);

// Check when active piece has landed
bool board_check_collision(struct Board *b);
// Modify b->layout to # where b->active currently is
void board_place_active(struct Board *b);
// Move active block and check for collisions
// Return true if moved else false
bool board_move_active(struct Board *b, vec3 dir);
void board_clear_full_lines(struct Board *b);

void board_add_piece(struct Board *b, struct Piece *p);
struct Piece *board_spawn_piece(struct Board *b);

void board_swap_hold(struct Board *b);

void board_make_borders(struct Board *b);

#endif

