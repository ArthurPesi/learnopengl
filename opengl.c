#include <stdlib.h>
#include <unistd.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "src/stb_image.h"
#include "pesiutils.c"
#define InputJustPressed(button) ((button).isDown == 1 && (button).halfTransitionCount == 1)
#define InputPressed(button) ((button).isDown == 1)

#define InputJustReleased(button) ((button).isDown == 0 && (button).halfTransitionCount == 1)
#define InputIsReleased(button) ((button).isDown == 0)

#define magnitude(pos, neg) ((pos) - (neg))
#define inputVector(pos, neg) (InputPressed((pos)) - InputPressed((neg))) 

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
    buttonState esc;
} input;

typedef struct {
    real32 x;
    real32 y;
    real32 z;
} Vector3;

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
    struct {
        real32 x;
        real32 y;
        real32 z;
    };
    real32 components[3];
} Vec3f;

typedef union {
    Vec4f vectors[4];
    real32 components[4][4];
} Matrix4f;

//functions
//

void getNormalizedVector(real32 *vector, real32 *result, size_t size) {
    real32 magnitude = 0;
    for(size_t i = 0; i < size; i++) {
        magnitude += vector[i] * vector[i];
    }
    magnitude = (real32) sqrt(magnitude);

    if(result == NULL) {
        result = vector;
    }

    for(size_t i = 0; i < size; i++) {
        result[i] = vector[i] / magnitude;
    }
}

#define normalize(vector) getNormalizedVector(vector, NULL, arrayCount(vector))

void crossProduct(real32* v1, real32* v2, real32* result) {
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

Vec4f transform4(Matrix4f matrix, Vec4f vector) {
    Vec4f result;

    for(int i = 0; i < 4; i++) {
        real32 sum = 0;
        for(int j = 0; j < 4; j++) {
            sum += matrix.components[j][i] * vector.components[i];
        }
        result.components[i] = sum;
    }
    return result;
}

Matrix4f compose4(Matrix4f m1, Matrix4f m2) {
    Matrix4f result;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            real32 sum = 0;
            for(int k = 0; k < 4; k++) {
                sum += m1.components[k][j] * m2.components[i][k];
            }
            result.components[i][j] = sum;
        }
    }
    return result;
}

Matrix4f identityMatrix4(void) {
    Matrix4f result;
    memset(&result, '\0', sizeof(result));
    for(int i = 0; i < 4; i++) {
        result.components[i][i] = 1;
    }
    return result;
}

Matrix4f translate4(real32 x, real32 y, real32 z) {
    Matrix4f result = identityMatrix4();
    result.components[3][0] = -x;
    result.components[3][1] = -y;
    result.components[3][2] = -z;
    result.components[3][3] = 1;
    return result;
}

Matrix4f scaleMatrix4(real32 scale) {
    Matrix4f result;
    memset(&result, '\0', sizeof(result));
    for(int i = 0; i < 3; i++) {
        result.components[i][i] = scale;
    }
    result.components[3][3] = 1;
    return result;
}

Matrix4f createProjection(real32 angle, real32 aspectRatio, real32 near, real32 far) {
    const real32 tangent = tan(angle/2);
    Matrix4f projectionMatrix;
    projectionMatrix.components[0][0] =  1 / (aspectRatio * tangent);
    projectionMatrix.components[0][1] = 0;
    projectionMatrix.components[0][2] = 0;
    projectionMatrix.components[0][3] = 0;

    projectionMatrix.components[1][0] = 0;
    projectionMatrix.components[1][1] = 1 / tangent;
    projectionMatrix.components[1][2] = 0;
    projectionMatrix.components[1][3] = 0;

    projectionMatrix.components[2][0] = 0;
    projectionMatrix.components[2][1] = 0;
    projectionMatrix.components[2][2] = (-(far + near)) / (far - near);
    projectionMatrix.components[2][3] = -1;

    projectionMatrix.components[3][0] = 0;
    projectionMatrix.components[3][1] = 0;
    projectionMatrix.components[3][2] = -(2 * far * near) / (far - near);
    projectionMatrix.components[3][3] = 0;

    return projectionMatrix;
}

void processInput(GLFWwindow* window, input* curr, input* prev) {
    if(glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
    curr->esc.isDown = glfwGetKey(window, GLFW_KEY_ESCAPE);
    curr->esc.halfTransitionCount = (curr->esc.isDown != prev->esc.isDown);

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

void enter_fullscreen(GLFWwindow *window, GLFWmonitor* monitor) {
    if(monitor && window) {
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } 
}


void exit_fullscreen(GLFWwindow *window, int width, int height) {
    glfwSetWindowMonitor(window, NULL, 0, 0, width, height, 0);
}

int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1000, 1000, "LearnOpenGL", NULL, NULL);
    p_assert(window != NULL); //TODO: testar glfwTerminate
    glfwMakeContextCurrent(window);

    //TODO: fazer o modo negativo se for suportado e deixar desativar o vsync (glfwSwapInterval)
    p_assert( gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress) );

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    enter_fullscreen(window, monitor);
    bool8 fullscreen = true;
    glViewport(0,0, 1060, 600);

    //Opengl configuration goes here
    glEnable(GL_DEPTH_TEST);
    uint32_t vertexShader = pesiLoadAndCompileShader(GL_VERTEX_SHADER, "vertex.glsl");
    uint32_t fragmentShader = pesiLoadAndCompileShader(GL_FRAGMENT_SHADER, "fragment.glsl");

    uint32_t shaderProgram = glCreateProgram();
    p_assert( shaderProgram != 0 );

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int32_t success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512] = "Error during program linking ";
        glGetProgramInfoLog(shaderProgram, 512 - 29, NULL, &infoLog[29]);
        throwError(infoLog);
    }

    glUseProgram(shaderProgram);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //framebuffer_size_callback(window, 800, 600);

    int32_t image_width, image_height, image_nrChannels;
    uint8_t* texture_data = stbi_load("toddy1.jpeg", &image_width, &image_height, &image_nrChannels, 0);
    const real32 imageRatio = (real32) image_height / image_width;
    p_assert( texture_data != 0 );
    const real32 vertexWidth = 1.7f;
    const real32 vertexHeight = vertexWidth * imageRatio;

    //TODO: criar um retangulo em espaco local e conseguir mexer ele
    Vector3 verticesRectangle[] = {
        {vertexWidth, vertexHeight, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
        {vertexWidth, -vertexHeight, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
        {-vertexWidth, -vertexHeight, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},
        {-vertexWidth, vertexHeight, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f},
    };
    float verticesCube[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
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
    glBufferData(GL_ARRAY_BUFFER, sizeof( verticesCube ), (void *) verticesCube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( real32 ) * 5, (void*) 0); 
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof( real32 ) * 5, (void*) (sizeof( real32 ) * 3)); 
    glEnableVertexAttribArray(1);

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

    int32_t modo_poggers = GL_MIRRORED_REPEAT;
    real32 borderColor[] = {0.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modo_poggers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modo_poggers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



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
    int projectionUniform = glGetUniformLocation(shaderProgram, "projection");
    int modelUniform = glGetUniformLocation(shaderProgram, "model");
    int viewUniform = glGetUniformLocation(shaderProgram, "view");
    int playerUniform = glGetUniformLocation(shaderProgram, "player");
    real32 cameraPos[] = {0.0f, 0.0f, 10.0f};
    real32 cameraTarget[] = {0.0f, 0.0f, 0.0f};

    real32 rot = 0.0f;
    input keyboards[2];
    memset(keyboards, '\0', sizeof( keyboards )); 
    input* inputBuffer1 = &keyboards[0];
    input* inputBuffer2 = &keyboards[1];
    input* keyboard = inputBuffer1;
    input* lastKeyboard;
    
    const real32 nearPlane = 1.0f;
    const real32 farPlane = 100.0f;
    const real32 verticalFov = radians(45.0f);
    const real32 upVector[] = {0.0f, 1.0f, 0.0f};
    real32 cubeLocations[5][3] = {{0.0f, 0.0f, 0.0f},
                                    {3.0f, -2.0f, 7.0f},
                                    {-1.0f, 4.0f, 12.0f},
                                    {-3.0f, -2.0f, 10.0f},
                                    {0.0f, 0.0f, 5.0f} };


    //main loop
    while(!glfwWindowShouldClose(window)) {
        lastKeyboard = keyboard;
        keyboard = (keyboard == inputBuffer1) ? inputBuffer2 : inputBuffer1;
        processInput(window, keyboard, lastKeyboard);
        if(InputJustPressed(keyboard->esc)) {
            if(fullscreen) {
                exit_fullscreen(window, 800, 600);
                fullscreen = false;
            } else {
                enter_fullscreen(window, monitor);
                fullscreen = true;
            }
        }
        #define SPEED 0.08f
        #define ROT_SPEED 0.06f

        cameraPos[0] -= SPEED * inputVector(keyboard->right, keyboard->left);
        cameraPos[1] -= SPEED * inputVector(keyboard->up, keyboard->down);
        cameraPos[2] -= SPEED * inputVector(keyboard->back, keyboard->front);
        real32 cameraFrontVector[3];
        getNormalizedVector(cameraPos, cameraFrontVector, arrayCount( cameraPos ) );
        real32 rightCameraVector[3];
        real32 upCameraVector[3];
        crossProduct(cameraFrontVector, (real32 *) upVector, rightCameraVector);
        normalize(rightCameraVector);
        crossProduct(cameraFrontVector, rightCameraVector, upCameraVector);
        //rot += ROT_SPEED * magnitude(InputPressed(keyboard->q), InputPressed(keyboard->e));
        Matrix4f cameraTranslation = translate4(cameraPos[0],cameraPos[1],cameraPos[2]);
        Matrix4f cameraRotation;
        memset(&cameraRotation, '\0', sizeof( cameraRotation) );
        for(int i = 0; i < 3; i++) {
            cameraRotation.components[i][0] = rightCameraVector[i];
            cameraRotation.components[i][1] = upCameraVector[i];
            cameraRotation.components[i][2] = cameraFrontVector[i];
        }
        cameraRotation.components[3][3] = 1;

        Matrix4f viewMatrix = compose4(cameraRotation, cameraTranslation);
        //viewMatrix.components[0][0] = 1.0f;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                printf("%f ", cameraRotation.components[j][i]);
            }
            printf("\n");
        }
        printf("finished\n");
        glUniformMatrix4fv(viewUniform, 1, GL_FALSE, (GLfloat *) &viewMatrix);

        //TODO: recalcular projecao so quando necessario
        int viewport_dimensions[4];
        glGetIntegerv(GL_VIEWPORT, viewport_dimensions);
        real32 aspectRatio = (real32) viewport_dimensions[2] / viewport_dimensions[3];

        Matrix4f projection = createProjection(verticalFov, aspectRatio, nearPlane, farPlane); 

        glUniform1f(timeUniform, glfwGetTime());
        glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, (GLfloat *) &projection);


        //render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for(int i = 0; i < 5; i++) {
            Matrix4f modelMatrix = translate4(cubeLocations[i][0], cubeLocations[i][1], cubeLocations[i][2]);
            glUniformMatrix4fv(modelUniform, 1, GL_FALSE, (GLfloat *) &modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);

        glfwSwapBuffers(window); //Swaps buffers only after monitor is done rendering (Enforces FPS)
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
