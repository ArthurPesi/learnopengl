#include <stdlib.h>
#include <unistd.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "pesiutils.c"

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0,0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

// static const char* vertexShaderText = "#version 330 core\n"
//                                     "layout (location = 0) in vec3 aPos;\n"
//                                     "void main()\n"
//                                     "{ gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }\0";
//
// static const char* fragmentShaderText = "#version 330 core\n"
//                                         "out vec4 FragColor;\n"
//                                         "void main()\n"
//                                         "{ FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); }\0";

typedef struct {
    float x;
    float y;
    float z;
} Vector3;


uint32_t pesiLoadAndCompileShader(uint32_t shaderType, char *file) {
    FILE *shaderFile = fopen(file, "r");
    assert(shaderFile != NULL);
    fseek(shaderFile, 0, SEEK_END);
    int32_t length = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);
    char *buffer = malloc(length);
    assert(buffer != NULL);
    fread(buffer, 1, length, shaderFile);
    fclose(shaderFile);
    

    uint32_t shader = glCreateShader(shaderType);
    assert( shader != 0 );

    const char *shaderSource = buffer;

    glShaderSource(shader, 1, &shaderSource, &length);
    glCompileShader(shader);

    int32_t success;//TODO: bool32
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512] = "Error during shader compilation ";
        glGetShaderInfoLog(shader, 480, NULL, &infoLog[32]);
        throwError(infoLog);
    }

    return shader;
}

int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1000, 1000, "LearnOpenGL", NULL, NULL);
    assert(window != NULL); //TODO: testar glfwTerminate
    glfwMakeContextCurrent(window);

    assert( gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress) );

    uint32_t vertexShader = pesiLoadAndCompileShader(GL_VERTEX_SHADER, "vertex.glsl");
    uint32_t fragmentShader = pesiLoadAndCompileShader(GL_FRAGMENT_SHADER, "fragment.glsl");
    // uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // assert( vertexShader != 0 );
    //
    // glShaderSource(vertexShader, 1, &vertexShaderText, NULL);
    // glCompileShader(vertexShader);
    //
    // int32_t success;//TODO: bool32
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // if(!success) {
    //     char infoLog[512] = "Error during vertex shader compilation ";
    //     glGetShaderInfoLog(vertexShader, 472, NULL, &infoLog[39]);
    //     throwError(infoLog);
    // }
    //
    // uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // assert( fragmentShader != 0 );
    //
    // glShaderSource(fragmentShader, 1, &fragmentShaderText, NULL);
    // glCompileShader(fragmentShader);
    //
    // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // if(!success) {
    //     char infoLog[512] = "Error during fragment shader compilation "; //TODO: string utility
    //     glGetShaderInfoLog(fragmentShader, 474, NULL, &infoLog[39]);
    //     throwError(infoLog);
    // }

    uint32_t shaderProgram = glCreateProgram();
    assert( shaderProgram != 0 );

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int32_t success;//TODO: bool32
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512] = "Error during program linking ";
        glGetProgramInfoLog(shaderProgram, 512 - 29, NULL, &infoLog[29]);
        throwError(infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    glViewport(0,0,800,600);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    Vector3 vertices[] = {
                            {0.5f, 0.5f, 0.0f},
                            {0.5f, -0.5f, 0.0f},
                            {-0.5f, -0.5f, 0.0f},
                            {-0.5f, 0.5f, 0.0f}
                        };

    uint32_t indices[] = {
        0,1,3,
        1,2,3 };

    uint32_t VBO;
    glGenBuffers(1, &VBO);


    uint32_t EBO;
    glGenBuffers(1, &EBO);
    
    uint32_t VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof( vertices ), (void *) vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( Vector3 ), (void*) 0); 
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW);


    glUseProgram(shaderProgram);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
