#include "prog.h"
#include <time.h>


int main()
{
    srand(time(0));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *win = glfwCreateWindow(1200, 900, "Opengl", 0, 0);
    glfwMakeContextCurrent(win);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, 1200, 900);

    while (true)
    {
        struct Prog *p = prog_alloc(win);
        prog_mainloop(p);
        bool restart = p->restart;
        prog_free(p);

        if (!restart)
            break;
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}

