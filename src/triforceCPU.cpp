#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "base/utils.h"

// Angle variation per frame for each axis.
constexpr static const float kDeltaAngle = 2.0F * PI / 100.0F;

// Number of entries that represent a single vertex.
static const size_t kDataPerVertex = 4;
// Total number of vertices present in the vertex buffer object.
static const size_t kNumVertices = 9;
// Triangle vertex positions in 4D clip-space. Each 4 entries represent a
// single.
static float sVboData[kNumVertices * kDataPerVertex] = {
    // clang-format off
    // Upper triangle
    0.0, 0.5, 0.0, 1.0,
    -0.25, 0.0, 0.0, 1.0,
    0.25, 0.0, 0.0, 1.0,
    // Down left triangle
    -0.25, 0.0, 0.0, 1.0,
    -0.5, -0.5, 0.0, 1.0,
    0.0, -0.5, 0.0, 1.0,
    // Down right triangle
    0.25, 0.0, 0.0, 1.0,
    0.0, -0.5, 0.0, 1.0,
    0.5, -0.5, 0.0, 1.0
    // clang-format on
};

/** @brief String representing the vertex shader. */
static const char *kVertexShaderStr =
    R"glsl(#version 460
layout(location = 0) in vec4 pos;

void main() {
    gl_Position = pos;
}
)glsl";

/** @brief String representing the fragment shader. */
static const char *kFragmentShaderStr =
    R"glsl(#version 460
out vec4 outCol;

void main() {
    outCol = vec4(1.0f, 0.843f, 0.0f, 1.0f);
}
)glsl";

/** @brief OpenGL program containing the vertex and fragment shader. */
static GLuint sGLProgram = 0;

/** @brief Vertex buffer object. */
static GLuint sVBO = 0;

/** @brief Vertex array object. */
static GLuint sVAO = 0;

/**
 * @brief Initializes the OpenGL program object `sGLProgram` by creating the
 *        shaders from `kVertexShaderStr` and `kFragmentShaderStr`. If the creation
 *        of the program fails, the function will toggle the GLFW window to close.
 *
 * @param window Window associated to the OpenGL context.
 */
bool initShaderProgram() {
    GLuint vertexShader, fragmentShader;
    if (!(utils::createShaderFromString(vertexShader, GL_VERTEX_SHADER, kVertexShaderStr) &&
          utils::createShaderFromString(fragmentShader, GL_FRAGMENT_SHADER, kFragmentShaderStr))) {
        return false;
    }
    GLuint shaders[2] = {vertexShader, fragmentShader};

    if (!utils::createProgram(sGLProgram, shaders, 2)) {
        return false;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

/**
 * @brief Takes the data from `sVBOData` and copies it into an OpenGL buffer
 *        `sVBO` in the GPU.
 */
void initBufferObjects() {
    glGenVertexArrays(1, &sVAO);
    glGenBuffers(1, &sVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sVboData), sVboData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief Rotate the vertices in `sVBOData` by given a given angle in each axis x, y and z.
 *
 * @param t Common rotation angle for each axis.
 */
void rotateVertices(float t) {
    for (size_t vertexIdx = 0; vertexIdx < kNumVertices; vertexIdx++) {
        size_t idx = kDataPerVertex * vertexIdx;
        float x = sVboData[idx];
        float y = sVboData[idx + 1];
        float z = sVboData[idx + 2];

        sVboData[idx] = (2.0F * x + 2.0F * z + y * cosf(t) + 2.0F * x * cosf(2.0F * t) -
                         2.0F * z * cosf(2.0F * t) - y * cosf(3.0F * t) + z * sinf(t) -
                         2.0F * y * sinf(2.0F * t) + z * sinf(3.0F * t)) /
                        4.0F;
        sVboData[idx + 1] = (2.0F * y + z * cosf(t) + 2.0F * y * cosf(2.0F * t) -
                             z * cosf(3.0F * t) + 3.0F * y * sinf(t) + 2.0F * x * sinf(2.0F * t) -
                             2.0F * z * sinf(2.0F * t) - y * sinf(3.0F * t)) /
                            4.0F;
        sVboData[idx + 2] =
            (z + z * cosf(2.0F * t) - 2.0F * x * sinf(t) + y * sinf(2.0F * t)) / 2.0F;
    }
}

/**
 * @brief Update vertex positions in `sVBO` and copies to the corresponding
 *        vertex buffer object.
 */
void updateScene() {
    rotateVertices(kDeltaAngle);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sVboData), &sVboData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief Clears the display, and using the `sGLProgram` program object and the
 *        `sVBO` buffer object, draws to the back buffer.
 */
void renderScene() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(sGLProgram);
    glBindVertexArray(sVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, kDataPerVertex, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, kNumVertices);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

/**
 * @brief Clean up the OpenGL objects when closing the window, and destroy the
 *        window.
 */
void windowCloseCallback(GLFWwindow *window) {
    printf("Deleting OpenGL objects...\n");
    glDeleteProgram(sGLProgram);
    glDeleteBuffers(1, &sVBO);
    glDeleteVertexArrays(1, &sVAO);

    printf("Closing window...\n");
    glfwDestroyWindow(window);
}

void terminate(GLFWwindow *window) {
    windowCloseCallback(window);
    glfwTerminate();
}

int main() {
    GLFWwindow *window = utils::initGLFW("Triforce CPU");
    utils::setGLFWCallbacks(
        window, utils::KEY_CALLBACK | utils::RESIZE_CALLBACK | utils::WINDOW_CLOSE_CALLBACK);
    glfwSwapInterval(1);

    if (!initShaderProgram()) {
        terminate(window);
        return -1;
    }
    initBufferObjects();

    double timer = 0.0;
    int fps = 0;
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        updateScene();
        renderScene();
        glfwSwapBuffers(window);
        fps++;

        if (glfwGetTime() - timer > 1.0) {
            timer++;
            printf("\r\x1b[A\x1b[2K");
            printf("FPS: %d\n", fps);
            fps = 0;
        }
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
