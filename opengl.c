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
    uint32_t gremioShader = pesiLoadAndCompileShader(GL_FRAGMENT_SHADER, "gremio.glsl");

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

    uint32_t gremioProgram = glCreateProgram();
    assert( gremioProgram != 0 );
    glAttachShader(gremioProgram, vertexShader);
    glAttachShader(gremioProgram, gremioShader);
    glLinkProgram(gremioProgram);

    glDeleteShader(gremioShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    glViewport(0,0,800,600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Vector3 vertices[] = {
                            {-0.25f, 0.0f, 0.0f},
                            {-0.75f, 0.0f, 0.0f},
                            {-0.5f, 0.25f, 0.0f},
                            {0.25f, 0.0f, 0.0f},
                            {0.75f, 0.0f, 0.0f},
                            {0.5f, 0.25f, 0.0f},
                        };

    // uint32_t indices[] = {
    //     1,4,2,
    //     4,2,5
    //      };

    uint32_t VBOS[2];
    glGenBuffers(2, VBOS);

    uint32_t VBO = VBOS[0];
    uint32_t VBO2 = VBOS[1];

    // uint32_t EBO;
    // glGenBuffers(1, &EBO);
    
    uint32_t VAOS[2];
    glGenVertexArrays(2, VAOS);
    const uint32_t VAO = VAOS[0];
    const uint32_t VAO2 = VAOS[1];

//First one
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof( Vector3 ) * 6, (void *) vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof( Vector3 ), (void*) 0); 
    glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW);

//Second one
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof( Vector3 ) * 3, (void *) (&vertices[3]), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof( Vector3 ), (void*) 0); 
    glEnableVertexAttribArray(0);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(VAO); 
    uint32_t curr = shaderProgram;
    uint32_t lastInput = GLFW_RELEASE;
    uint32_t currInput = GLFW_RELEASE;
    while(!glfwWindowShouldClose(window)) {
        processInput(window);
        currInput = glfwGetKey(window, GLFW_KEY_SPACE);
        if(currInput != GLFW_RELEASE && lastInput != GLFW_PRESS) {
            curr ^= shaderProgram ^ gremioProgram;
            glUseProgram(curr);
        }
        lastInput = currInput;

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
