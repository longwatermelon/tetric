#ifndef PROG_H
#define PROG_H

#include "shader.h"
#include "render.h"
#include "skybox.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Prog
{
    GLFWwindow *win;
    RenderInfo *ri;

    struct Camera *cam;

    struct Board *board;
    struct Skybox *skybox;
};

struct Prog *prog_alloc(GLFWwindow *win);
void prog_free(struct Prog *p);

void prog_mainloop(struct Prog *p);

void prog_rotate_cam(struct Prog *p);

#endif

