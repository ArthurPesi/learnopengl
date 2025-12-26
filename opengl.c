#include <stdlib.h>
#include <unistd.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define M_PI		3.14159265358979323846	/* pi */
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "src/stb_image.h"
#include "pesiutils.c"
#define InputJustPressed(button) ((button).isDown == 1 && (button).halfTransitionCount == 1)
#define InputPressed(button) ((button).isDown == 1)

#define InputJustReleased(button) ((button).isDown == 0 && (button).halfTransitionCount == 1)
#define InputIsReleased(button) ((button).isDown == 0)

#define magnitude(pos, neg) ((pos) - (neg))

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0,0, width, height);
}

typedef struct {
    bool8 isDown;
    uint8 halfTransitionCount;
} buttonState;

typedef struct {
    buttonState up;
    buttonState down;
    buttonState left;
    buttonState right;
    buttonState q;
    buttonState e;
    buttonState front;
    buttonState back;
    buttonState space;
} input;


typedef union {
    struct {
        real32 x;
        real32 y;
        real32 z;
        real32 w;
    };
    real32 components[4];
} Vec4f;

typedef union {
    Vec4f vectors[4];
    real32 components[4][4];
} Matrix4f;


void transform2d(double angle, real32 x, float y, float scale, Matrix4f* matrix) {
    double sin_ = sin(angle);
    double cos_ = cos(angle);
    matrix->components[0][0] = cos_ /scale;
    matrix->components[0][1] = 0;
    matrix->components[0][2] = sin_;
    matrix->components[0][3] = 0;

    matrix->components[1][0] = 0;
    matrix->components[1][1] = 1 / scale;
    matrix->components[1][2] = 0;
    matrix->components[1][3] = 0;

    matrix->components[2][0] = -sin_ / scale;
    matrix->components[2][1] = 0;
    matrix->components[2][2] = cos_;
    matrix->components[2][3] = 0;

    matrix->components[3][0] = -x;
    matrix->components[3][1] = -y;
    matrix->components[3][2] = 0;
    matrix->components[3][3] = 1;
}

void processInput(GLFWwindow* window, input* curr, input* prev) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
    curr->up.isDown = glfwGetKey(window, GLFW_KEY_W); 
    curr->up.halfTransitionCount = (curr->up.isDown != prev->up.isDown);

    curr->left.isDown = glfwGetKey(window, GLFW_KEY_A);
    curr->left.halfTransitionCount = (curr->left.isDown != prev->left.isDown);
    
    curr->down.isDown = glfwGetKey(window, GLFW_KEY_S);
    curr->down.halfTransitionCount = (curr->down.isDown != prev->down.isDown);

    curr->right.isDown = glfwGetKey(window, GLFW_KEY_D);
    curr->right.halfTransitionCount = (curr->right.isDown != prev->right.isDown);

    curr->space.isDown = glfwGetKey(window, GLFW_KEY_SPACE);
    curr->space.halfTransitionCount = (curr->space.isDown != prev->space.isDown);

    curr->q.isDown = glfwGetKey(window, GLFW_KEY_Q); 
    curr->q.halfTransitionCount = (curr->q.isDown != prev->q.isDown);

    curr->e.isDown = glfwGetKey(window, GLFW_KEY_E); 
    curr->e.halfTransitionCount = (curr->e.isDown != prev->e.isDown);
    
    curr->front.isDown = glfwGetKey(window, GLFW_KEY_I); 
    curr->front.halfTransitionCount = (curr->front.isDown != prev->front.isDown);

    curr->back.isDown = glfwGetKey(window, GLFW_KEY_K); 
    curr->back.halfTransitionCount = (curr->back.isDown != prev->back.isDown);
}

typedef struct {
    real32 x;
    real32 y;
    real32 z;
} Vector3;


uint32_t getNullPositionSize(char *str, uint32_t maxSize) {
    uint32_t count = 0;
    char i = 0;
    while((i = *(str++)) && ++count < maxSize) {}
    return count;
}

uint32_t pesiLoadAndCompileShader(uint32_t shaderType, char *file) {
    FILE *shaderFile = fopen(file, "r");
    p_assert(shaderFile != NULL);
    fseek(shaderFile, 0, SEEK_END);
    int32_t length = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);
    char *buffer = malloc(length);
    p_assert(buffer != NULL);
    fread(buffer, 1, length, shaderFile);
    fclose(shaderFile);
    

    uint32_t shader = glCreateShader(shaderType);
    p_assert( shader != 0 );

    const char *shaderSource = buffer;

    glShaderSource(shader, 1, &shaderSource, &length);
    glCompileShader(shader);

    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512] = "Error during shader compilation ";
        uint32_t stringSize = getNullPositionSize(infoLog, 512);
        glGetShaderInfoLog(shader, 512 - stringSize, NULL, &infoLog[stringSize]);
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
    p_assert(window != NULL); //TODO: testar glfwTerminate
    glfwMakeContextCurrent(window);

    p_assert( gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress) );


    uint32_t vertexShader = pesiLoadAndCompileShader(GL_VERTEX_SHADER, "vertex.glsl");
    uint32_t fragmentShader = pesiLoadAndCompileShader(GL_FRAGMENT_SHADER, "fragment.glsl");

    uint32_t shaderProgram = glCreateProgram();
    p_assert( shaderProgram != 0 );

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

    glUseProgram(shaderProgram);

    glViewport(0,0,800,600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Vector3 vertices[] = {
        {0.5f, 0.7f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
        {0.5f, -0.7f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
        {-0.5f, -0.7f, 1.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},
        {-0.5f, 0.7f, 2.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f},
    };
    uint32_t indices[] = {
        0,1,3,
        1,2,3 };

    uint32_t VBOS[1];
    glGenBuffers(1, VBOS);

    uint32_t VBO = VBOS[0];


    uint32_t VAOS[1];
    glGenVertexArrays(1, VAOS);
    const uint32_t VAO = VAOS[0];

    uint32_t EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof( Vector3 ) * 12, (void *) vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( Vector3 ) * 3, (void*) 0); 
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( Vector3 ) * 3, (void*) sizeof( Vector3 )); 
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof( Vector3 ) * 3, (void*) (sizeof( Vector3 ) * 2 ) ); 
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //

    uint32_t textures[2];
    glGenTextures(2, textures);

    uint32_t texture1 = textures[0];
    uint32_t texture2 = textures[1];

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    int32_t modo_poggers = GL_REPEAT;
    real32 borderColor[] = {0.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modo_poggers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modo_poggers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int32_t image_width, image_height, image_nrChannels;
    uint8_t* texture_data = stbi_load("toddy1.jpeg", &image_width, &image_height, &image_nrChannels, 0);
    p_assert( texture_data != 0 );


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modo_poggers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modo_poggers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uint8_t* texture_data2 = stbi_load("Untitled.jpg", &image_width, &image_height, &image_nrChannels, 0);
    p_assert( texture_data2 != 0 );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data2);
    glGenerateMipmap(GL_TEXTURE_2D);

    uint32_t texture1Uniform = glGetUniformLocation(shaderProgram, "texture1");
    uint32_t texture2Uniform = glGetUniformLocation(shaderProgram, "texture2");
    glUniform1i(texture1Uniform, 0);
    glUniform1i(texture2Uniform, 1);


    stbi_image_free(texture_data);
    stbi_image_free(texture_data2);

    int timeUniform = glGetUniformLocation(shaderProgram, "time");
    int rotMatrixUniform = glGetUniformLocation(shaderProgram, "transform");
    real32 x = 0.0f;
    real32 y = 0.0f;
    real32 rot = 0.0f;
    real32 scale = 1.0f;
    Matrix4f rotMatrix = {0};
    input keyboards[2];
    memset(keyboards, '\0', sizeof( input ) * 2); 
    input* inputBuffer1 = &keyboards[0];
    input* inputBuffer2 = &keyboards[1];
    input* keyboard = inputBuffer1;
    input* lastKeyboard;

    while(!glfwWindowShouldClose(window)) {
        lastKeyboard = keyboard;
        keyboard = (keyboard == inputBuffer1) ? inputBuffer2 : inputBuffer1;
        processInput(window, keyboard, lastKeyboard);
        #define SPEED 0.02f
        #define ROT_SPEED 0.06f
        x += SPEED * magnitude(InputPressed(keyboard->right), InputPressed(keyboard->left));
        y += SPEED * magnitude(InputPressed(keyboard->up), InputPressed(keyboard->down));
        rot += ROT_SPEED * magnitude(InputPressed(keyboard->q), InputPressed(keyboard->e));
        scale += SPEED * magnitude(InputPressed(keyboard->back), InputPressed(keyboard->front));

        if(InputJustPressed(keyboard->space)) {
            rot += M_PI;
        }
        
        transform2d(rot, x, y, scale, &rotMatrix);
        glUniform1f(timeUniform, glfwGetTime());
        glUniformMatrix4fv(rotMatrixUniform, 1, GL_FALSE, (GLfloat *) &rotMatrix);

        glClear(GL_COLOR_BUFFER_BIT);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
