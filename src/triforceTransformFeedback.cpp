#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <stdio.h>
#include <unistd.h>

#include "base/utils.h"

// Number of entries that represent a single vertex.
static const size_t kDataPerVertex = 3;
// Total number of vertices present in the vertex buffer object.
static const size_t kNumVertices = 9;
// Triangle vertex positions in 3D space. Each 3 entries represent a single.
static const float kInitialVertexData[kNumVertices * kDataPerVertex] = {
    // clang-format off
    // Upper triangle
    0.0, 0.5, 0.0,
    -0.25, 0.0, 0.0,
    0.25, 0.0, 0.0,
    // Down left triangle
    -0.25, 0.0, 0.0,
    -0.5, -0.5, 0.0,
    0.0, -0.5, 0.0,
    // Down right triangle
    0.25, 0.0, 0.0,
    0.0, -0.5, 0.0,
    0.5, -0.5, 0.0,
    // clang-format on
};
static const size_t kVertexDataSize = sizeof(kInitialVertexData);

// Vertex shader in raw string representation.
static const char* sVertexShaderStr =
    R"glsl(#version 460
layout(location = 0) in vec3 inPos;
layout(location = 0) uniform uint mode;
layout(location = 0) out vec3 outPos;

const float phi = 2.0 * 3.14159 / 100;

void main() {
    if (mode == 0) {
        outPos.x =
            (2.0 * inPos.x + 2.0 * inPos.z + inPos.y * cos(phi) + 2.0 * inPos.x * cos(2.0 * phi) -
             2.0 * inPos.z * cos(2.0 * phi) - inPos.y * cos(3.0 * phi) + inPos.z * sin(phi) -
             2.0 * inPos.y * sin(2.0 * phi) + inPos.z * sin(3.0 * phi)) /
            4.0;
        outPos.y =
            (2.0 * inPos.y + inPos.z * cos(phi) + 2.0 * inPos.y * cos(2.0 * phi) -
             inPos.z * cos(3.0 * phi) + 3.0 * inPos.y * sin(phi) + 2.0 * inPos.x * sin(2.0 * phi) -
             2.0 * inPos.z * sin(2.0 * phi) - inPos.y * sin(3.0 * phi)) /
            4.0;
        outPos.z = (inPos.z + inPos.z * cos(2.0 * phi) - 2.0 * inPos.x * sin(phi) +
                    inPos.y * sin(2.0 * phi)) /
                   2.0;
    } else {
        gl_Position = vec4(inPos, 1.0);
    }
})glsl";

// Update mode for the vertex shader by setting `mode` to `kModeRender`.
static const GLint kModeUpdate = 0;

// Render mode for the vertex shader by setting `mode` to `kModeRender`.
static const GLint kModeRender = 1;

// Layout location of the `mode` uniform attribute of the vertex shader.
static GLuint sModeUniformLoc = 0;

// Layout location of the `inPos` input attribute of the vertex shader.
static GLuint sInPosAttribLoc = 0;

// Layout location of the `outPos` output attribute of the vertex shader.
static GLuint sOutPosAttribLoc = 0;

// Fragment shader in raw string representation.
static const char* sFragmentShaderStr =
    R"glsl(#version 460
out vec4 outCol;

void main() {
    outCol = vec4(1.0, 0.843, 0.0, 1.0);
})glsl";

// OpenGL program containing the vertex and fragment shader.
static GLuint sGLProgram = 0;

// Vertex buffer object.
static GLuint sVBO = 0;

// Transform feedback vertex buffer object.
static GLuint sTBO = 0;

// Vertex array object.
static GLuint sVAO = 0;

/**
 * @brief Initializes the OpenGL program object `sGLProgram` by creating the shaders
 *        from `sVertexShaderStr` and `sFragmentShaderStr`. If the creation of the
 *        program fails, the function will toggle the GLFW window to close.
 *
 * @param window Window associated to the OpenGL context.
 */
bool initShaderProgram() {
    GLuint vertexShader, fragmentShader;
    if (!(utils::createShaderFromString(vertexShader, GL_VERTEX_SHADER, sVertexShaderStr) &&
          utils::createShaderFromString(fragmentShader, GL_FRAGMENT_SHADER, sFragmentShaderStr))) {
        return false;
    }

    sGLProgram = glCreateProgram();
    if (sGLProgram == 0) {
        fprintf(stderr, "Unable to create program object.\n");
        return false;
    }
    glAttachShader(sGLProgram, vertexShader);
    glAttachShader(sGLProgram, fragmentShader);

    const char* varyings[1] = {"outPos"};
    glTransformFeedbackVaryings(sGLProgram, 1, varyings, GL_INTERLEAVED_ATTRIBS);
    utils::linkProgram(sGLProgram);

    if (!(utils::findAttribLocation(sGLProgram, sModeUniformLoc, "mode", true) &&
          utils::findAttribLocation(sGLProgram, sInPosAttribLoc, "inPos", false))) {
        fprintf(stderr, "Unable to find attribute location.\n");
        return false;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void initBufferObjects() {
    glGenVertexArrays(1, &sVAO);
    glBindVertexArray(sVAO);

    // Create the vertex buffer and declare how much data will it carry.
    glGenBuffers(1, &sVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBufferData(GL_ARRAY_BUFFER, kVertexDataSize, nullptr, GL_DYNAMIC_DRAW);

    // Create the tranform feedback buffer and copy the initial positions of vertices.
    glGenBuffers(1, &sTBO);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, sTBO);
    glBufferData(
        GL_TRANSFORM_FEEDBACK_BUFFER, kVertexDataSize, kInitialVertexData, GL_DYNAMIC_DRAW);
}

void renderScene() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(sGLProgram);

    // Update the state of the vertex buffer with the previous trasform feedback.
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, sOutPosAttribLoc, sTBO);
    glCopyBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, GL_ARRAY_BUFFER, 0, 0, kVertexDataSize);

    glEnableVertexAttribArray(sInPosAttribLoc);
    glVertexAttribPointer(sInPosAttribLoc, kDataPerVertex, GL_FLOAT, GL_FALSE, 0, 0);

    // Update the transform feedback buffer, without rendering.
    glUniform1ui(static_cast<GLint>(sModeUniformLoc), kModeUpdate);
    glEnable(GL_RASTERIZER_DISCARD);
    {
        glBeginTransformFeedback(GL_TRIANGLES);
        glDrawArrays(GL_TRIANGLES, 0, kNumVertices);
        glEndTransformFeedback();
    }
    glDisable(GL_RASTERIZER_DISCARD);

    // Render the newly obtained data in the transform feedback buffer.
    glUniform1ui(static_cast<GLint>(sModeUniformLoc), kModeRender);
    glBindBuffer(GL_ARRAY_BUFFER, sTBO);
    glDrawArrays(GL_TRIANGLES, 0, kNumVertices);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void terminateRenderer() {
    fprintf(stderr, "Deleting OpenGL objects...\n");
    glEndTransformFeedback();
    glDeleteTransformFeedbacks(1, &sTBO);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &sVAO);
    glDeleteBuffers(1, &sVBO);
    glDeleteProgram(sGLProgram);
}

void terminate(GLFWwindow* window) {
    utils::windowCloseCallbackGLFW(window);
    terminateRenderer();
    glfwTerminate();
}

int main() {
    GLFWwindow* window = utils::initGLFW("Triforce Transform Feedback");
    utils::setGLFWCallbacks(
        window, utils::KEY_CALLBACK | utils::RESIZE_CALLBACK | utils::WINDOW_CLOSE_CALLBACK);
    glfwSwapInterval(1);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(utils::errorCallbackGL, 0);

    if (!initShaderProgram()) {
        utils::windowCloseCallbackGLFW(window);
        terminateRenderer();
        glfwTerminate();
        terminate(window);
    }

    initBufferObjects();

    double timer = 0.0;
    int fps = 0;
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
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
    terminateRenderer();
    glfwTerminate();

    return 0;
}
