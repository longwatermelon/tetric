#include "prog.h"
#include "board.h"
#include <stb/stb_image.h>
#include <stdlib.h>


struct Prog *prog_alloc(GLFWwindow *win)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->win = win;

    p->cam = cam_alloc((vec3){ 0.f, 0.f, 0.f }, (vec3){ 0.f, 0.f, 0.f });

    p->ri = ri_alloc();
    ri_add_shader(p->ri, "shaders/basic_v.glsl", "shaders/basic_f.glsl");

    p->ri->cam = p->cam;

    stbi_set_flip_vertically_on_load(true);

    p->board = board_alloc();

    return p;
}


void prog_free(struct Prog *p)
{
    board_free(p->board);
    cam_free(p->cam);
    free(p);
}


void prog_mainloop(struct Prog *p)
{
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

        prog_events(p);

        board_update(p->board);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ri_use_shader(p->ri, SHADER_BASIC);

        cam_set_props(p->cam, p->ri->shader);
        cam_view_mat(p->cam, p->ri->view);

        shader_mat4(p->ri->shader, "view", p->ri->view);
        shader_mat4(p->ri->shader, "projection", p->ri->proj);

        /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
        board_render(p->board, p->ri);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }
}


void prog_events(struct Prog *p)
{
    static float last_key = 0.f;

    if (glfwGetTime() - last_key < .05f)
        return;

    last_key = glfwGetTime();

    if (glfwGetKey(p->win, GLFW_KEY_LEFT) == GLFW_PRESS)
        board_move_active(p->board, (vec3){ 0.f, 0.f, -1.f });

    if (glfwGetKey(p->win, GLFW_KEY_RIGHT) == GLFW_PRESS)
        board_move_active(p->board, (vec3){ 0.f, 0.f, 1.f });


    /* float move = .05f; */

    /* vec3 front; */
    /* glm_vec3_scale(p->cam->front, move, front); */
    /* front[1] = 0.f; */

    /* vec3 right; */
    /* glm_vec3_scale(p->cam->right, move, right); */
    /* right[1] = 0.f; */

    /* if (glfwGetKey(p->win, GLFW_KEY_W) == GLFW_PRESS) glm_vec3_add(p->cam->pos, front, p->cam->pos); */
    /* if (glfwGetKey(p->win, GLFW_KEY_S) == GLFW_PRESS) glm_vec3_sub(p->cam->pos, front, p->cam->pos); */
    /* if (glfwGetKey(p->win, GLFW_KEY_A) == GLFW_PRESS) glm_vec3_sub(p->cam->pos, right, p->cam->pos); */
    /* if (glfwGetKey(p->win, GLFW_KEY_D) == GLFW_PRESS) glm_vec3_add(p->cam->pos, right, p->cam->pos); */

    /* if (glfwGetKey(p->win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) p->cam->pos[1] -= move; */
    /* if (glfwGetKey(p->win, GLFW_KEY_SPACE) == GLFW_PRESS) p->cam->pos[1] += move; */
}

