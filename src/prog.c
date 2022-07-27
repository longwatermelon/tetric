#include "prog.h"
#include "board.h"
#include "texture.h"
#include <stb/stb_image.h>
#include <stdlib.h>

struct Prog *g_prog;

static void key_callback(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_LEFT)
            board_move_active(g_prog->board, (vec3){ 0.f, 0.f, -1.f });

        if (key == GLFW_KEY_RIGHT)
            board_move_active(g_prog->board, (vec3){ 0.f, 0.f, 1.f });

        if (key == GLFW_KEY_DOWN)
            board_move_active(g_prog->board, (vec3){ 0.f, -1.f, 0.f });

        if (key == GLFW_KEY_UP && g_prog->board->active)
            piece_rotate(g_prog->board->active);

        if (key == GLFW_KEY_SPACE)
        {
            while (board_move_active(g_prog->board, (vec3){ 0.f, -1.f, 0.f }))
                ;

            g_prog->board->last_moved = glfwGetTime() - .5f;
            g_prog->last_shake = glfwGetTime();
        }

        if (key == GLFW_KEY_R)
            g_prog->rotate = !g_prog->rotate;

        if (key == GLFW_KEY_0)
        {
            glm_vec3_zero(g_prog->cam->pos);
            glm_vec3_zero(g_prog->cam->rot);
            cam_update_vectors(g_prog->cam);
        }

        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_7) g_prog->board->next_piece = key - GLFW_KEY_1;

        if (key == GLFW_KEY_G)
        {
            g_prog->use_normal_map = !g_prog->use_normal_map;
        }

        if (key == GLFW_KEY_Q)
        {
            g_prog->restart = true;
            g_prog->running = false;
        }

        if (key == GLFW_KEY_C)
        {
            board_swap_hold(g_prog->board);
        }
    }
}

struct Prog *prog_alloc(GLFWwindow *win)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;
    p->win = win;

    p->cam = cam_alloc((vec3){ 0.f, 0.f, 0.f }, (vec3){ 0.f, 0.f, 0.f });

    p->ri = ri_alloc();
    ri_add_shader(p->ri, "shaders/basic_v.glsl", "shaders/basic_f.glsl");
    ri_add_shader(p->ri, "shaders/skybox_v.glsl", "shaders/skybox_f.glsl");

    p->ri->cam = p->cam;

    p->board = board_alloc();
    p->skybox = skybox_alloc("res/bridge-skyboxes/Bridge/");

    p->rotate = false;
    p->use_normal_map = true;

    p->restart = false;

    p->last_shake = -100.f;

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

    glfwSetCursorPos(p->win, 600.f, 450.f);
    glfwSetInputMode(p->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double prev_mx, prev_my;
    glfwGetCursorPos(p->win, &prev_mx, &prev_my);

    struct Texture *norm_map = tex_alloc("res/normal.jpg");

    while (p->running && !glfwWindowShouldClose(p->win))
    {
        double mx, my;
        glfwGetCursorPos(p->win, &mx, &my);

        cam_rot(p->cam, (vec3){ 0.f, -(my - prev_my) / 100.f, -(mx - prev_mx) / 100.f });
        prev_mx = mx;
        prev_my = my;

        board_update(p->board);

        if (p->rotate)
            prog_rotate_cam(p);

        if (glfwGetTime() - p->last_shake < .05f)
            cam_shake(p->cam);
        else
            glm_vec3_zero(p->cam->shake);

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
        shader_int(p->ri->shader, "use_normal_map", p->use_normal_map);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, p->skybox->tex);
        shader_int(p->ri->shader, "skybox", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, norm_map->id);
        shader_int(p->ri->shader, "norm_map", 1);

        board_render(p->board, p->ri);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }

    tex_free(norm_map);
}


void prog_rotate_cam(struct Prog *p)
{
    float angle = .06f;
    vec3 center = { 30.f, 0.f, 0.f };
    float dist = glm_vec3_distance(p->cam->pos, center);

    cam_rot(p->cam, (vec3){ 0.f, 0.f, angle / 2.f });

    vec3 move;
    glm_vec3_scale(p->cam->right, 2.f * dist * sinf(angle / 2.f), move);

    glm_vec3_add(p->cam->pos, move, p->cam->pos);
    cam_rot(p->cam, (vec3){ 0.f, 0.f, angle / 2.f });

    vec3 back;
    glm_vec3_negate_to(p->cam->front, back);
    glm_vec3_scale(back, 30.f, back);
    glm_vec3_add(center, back, p->cam->pos);
}

