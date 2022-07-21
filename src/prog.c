#include "prog.h"
#include "board.h"
#include <stb/stb_image.h>
#include <stdlib.h>

struct Prog *g_prog;

#define GLFW_REPEAT 2

static void key_callback(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        board_move_active(g_prog->board, (vec3){ 0.f, 0.f, -1.f });

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        board_move_active(g_prog->board, (vec3){ 0.f, 0.f, 1.f });

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        board_move_active(g_prog->board, (vec3){ 0.f, -1.f, 0.f });

    if (key == GLFW_KEY_UP && action == GLFW_PRESS && g_prog->board->active)
        piece_rotate(g_prog->board->active);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        while (board_move_active(g_prog->board, (vec3){ 0.f, -1.f, 0.f }))
            ;
    }
}

struct Prog *prog_alloc(GLFWwindow *win)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->win = win;

    p->cam = cam_alloc((vec3){ 0.f, 0.f, 0.f }, (vec3){ 0.f, 0.f, 0.f });

    p->ri = ri_alloc();
    ri_add_shader(p->ri, "shaders/basic_v.glsl", "shaders/basic_f.glsl");
    ri_add_shader(p->ri, "shaders/skybox_v.glsl", "shaders/skybox_f.glsl");

    p->ri->cam = p->cam;

    p->board = board_alloc();
    p->skybox = skybox_alloc("res/skybox/");

    g_prog = p;
    return p;
}


void prog_free(struct Prog *p)
{
    skybox_free(p->skybox);
    board_free(p->board);
    cam_free(p->cam);
    ri_free(p->ri);
    free(p);
}


void prog_mainloop(struct Prog *p)
{
    glfwSetKeyCallback(p->win, key_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glfwSetCursorPos(p->win, 400.f, 300.f);
    glfwSetInputMode(p->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double prev_mx, prev_my;
    glfwGetCursorPos(p->win, &prev_mx, &prev_my);

    while (!glfwWindowShouldClose(p->win))
    {
        double mx, my;
        glfwGetCursorPos(p->win, &mx, &my);

        cam_rot(p->cam, (vec3){ 0.f, -(my - prev_my) / 100.f, -(mx - prev_mx) / 100.f });
        prev_mx = mx;
        prev_my = my;

        board_update(p->board);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ri_use_shader(p->ri, SHADER_SKYBOX);
        skybox_render(p->skybox, p->ri);

        ri_use_shader(p->ri, SHADER_BASIC);

        cam_set_props(p->cam, p->ri->shader);
        cam_view_mat(p->cam, p->ri->view);

        shader_mat4(p->ri->shader, "view", p->ri->view);
        shader_mat4(p->ri->shader, "projection", p->ri->proj);
        shader_vec3(p->ri->shader, "cam_pos", p->ri->cam->pos);

        glBindTexture(GL_TEXTURE_CUBE_MAP, p->skybox->tex);
        board_render(p->board, p->ri);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }
}

