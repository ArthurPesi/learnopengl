#include "pesiutils.c"
#include <unistd.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void open_window(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    assert(window != NULL); //TODO: testar terminate
    glfwMakeContextCurrent(window);
    assert( gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress) );

    glViewport(0,0,800,600);
}

int main(void) {
    open_window();
    glfwTerminate();
    return 0;
}
