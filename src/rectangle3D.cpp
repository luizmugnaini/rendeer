#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <stdio.h>

#include "base/utils.h"

// Total number of vertices in the scene.
const size_t kNumVertices = 36;

// Number of values that define the position of a single vertex.
const size_t kPositionDataPerVertex = 3;

// Number of values that define the color of a single vertex.
const size_t kColorDataPerVertex = 3;

// Array containing both the initial position data of each vertex composing the scene and the color
// of each vertex. The data is stored in the following way:
// - Vertex positions: first `kNumVertices * kPositionDataPerVertex` values.
// - Vertex colors: last `kNumVertices * kColorDataPerVertex` values.
const float kInitialVertexData[(kPositionDataPerVertex + kColorDataPerVertex) * kNumVertices] = {
    // clang-format off
    /**************
     * Vertex data.
     **************/
	 0.25f,  0.25f, -1.25f,
	 0.25f, -0.25f, -1.25f,
	-0.25f,  0.25f, -1.25f,

	 0.25f, -0.25f, -1.25f,
	-0.25f, -0.25f, -1.25f,
	-0.25f,  0.25f, -1.25f,

	 0.25f,  0.25f, -2.75f,
	-0.25f,  0.25f, -2.75f,
	 0.25f, -0.25f, -2.75f,

	 0.25f, -0.25f, -2.75f,
	-0.25f,  0.25f, -2.75f,
	-0.25f, -0.25f, -2.75f,

	-0.25f,  0.25f, -1.25f,
	-0.25f, -0.25f, -1.25f,
	-0.25f, -0.25f, -2.75f,

	-0.25f,  0.25f, -1.25f,
	-0.25f, -0.25f, -2.75f,
	-0.25f,  0.25f, -2.75f,

	 0.25f,  0.25f, -1.25f,
	 0.25f, -0.25f, -2.75f,
	 0.25f, -0.25f, -1.25f,

	 0.25f,  0.25f, -1.25f,
	 0.25f,  0.25f, -2.75f,
	 0.25f, -0.25f, -2.75f,

	 0.25f,  0.25f, -2.75f,
	 0.25f,  0.25f, -1.25f,
	-0.25f,  0.25f, -1.25f,

	 0.25f,  0.25f, -2.75f,
	-0.25f,  0.25f, -1.25f,
	-0.25f,  0.25f, -2.75f,

	 0.25f, -0.25f, -2.75f,
	-0.25f, -0.25f, -1.25f,
	 0.25f, -0.25f, -1.25f,

	 0.25f, -0.25f, -2.75f,
	-0.25f, -0.25f, -2.75f,
	-0.25f, -0.25f, -1.25f,

    /*************
     * Color data.
     *************/
	0.0F, 0.0F, 1.0F,
	0.0F, 0.0F, 1.0F,
	0.0F, 0.0F, 1.0F,

	0.0F, 0.0F, 1.0F,
	0.0F, 0.0F, 1.0F,
	0.0F, 0.0F, 1.0F,

	0.8F, 0.8F, 0.8F,
	0.8F, 0.8F, 0.8F,
	0.8F, 0.8F, 0.8F,

	0.8F, 0.8F, 0.8F,
	0.8F, 0.8F, 0.8F,
	0.8F, 0.8F, 0.8F,

	0.0F, 1.0F, 0.0F,
	0.0F, 1.0F, 0.0F,
	0.0F, 1.0F, 0.0F,

	0.0F, 1.0F, 0.0F,
	0.0F, 1.0F, 0.0F,
	0.0F, 1.0F, 0.0F,

	0.5F, 0.5F, 0.0F,
	0.5F, 0.5F, 0.0F,
	0.5F, 0.5F, 0.0F,

	0.5F, 0.5F, 0.0F,
	0.5F, 0.5F, 0.0F,
	0.5F, 0.5F, 0.0F,

	1.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,

	1.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,

	0.0F, 1.0F, 1.0F,
	0.0F, 1.0F, 1.0F,
	0.0F, 1.0F, 1.0F,

	0.0F, 1.0F, 1.0F,
	0.0F, 1.0F, 1.0F,
	0.0F, 1.0F, 1.0F,
    // clang-format on
};

// Size of the array containing the data of the vertices (positions and colors).
const size_t kVertexDataSize = sizeof(kInitialVertexData);

// Offset into the array of vertex data that points to the color data of the vertices (last half).
const size_t kColorDataOffset = kVertexDataSize / 2;

// Vertex array object.
static GLuint sVAO = 0;

// Vertex buffer object.
static GLuint sVBO = 0;

// Program object.
static GLuint sGLProgram = 0;

// String representation of the vertex shader.
static const char* kVertexShaderStr =
    R"glsl(#version 460
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inCol;

layout(location = 0) uniform mat4 perspectiveMat;
layout(location = 1) uniform vec2 cameraOffset;


layout(location = 0) out vec3 outCol;

void main() {
    outCol = inCol;
    vec4 cameraPos = vec4(inPos + vec3(cameraOffset, 0.0), 1.0);
    gl_Position =  perspectiveMat * cameraPos;
}
)glsl";

// String representation of the fragment shader.
static const char* kFragmentShaderStr =
    R"glsl(#version 460
layout(location = 0) in vec3 inCol;
out vec4 outCol;

void main() {
    outCol = vec4(inCol, 1.0);
}
)glsl";

// Input attributes
static GLuint sInPosLoc = 0;
static GLuint sInColLoc = 0;

// Uniform inputs
static GLuint sPerspectiveMatLoc = 0;
static GLuint sCameraOffsetLoc = 0;

static const float kFrustumScale = 1.0F;
static const float kZCameraNear = 0.5F;
static const float kZCameraFar = 3.0F;
static const float kCameraOffset[2] = {1.5F, 0.5F};

// Matrix used for performing the perspective projection in the vertex shader.
static float sPerspectiveMat[16] = {
    // clang-format off
    kFrustumScale, 0.0F,          0.0F,                                                            0.0F,
    0.0F,          kFrustumScale, 0.0F,                                                            0.0F,
    0.0F,          0.0F,          (kZCameraNear + kZCameraFar) / (kZCameraNear - kZCameraFar),    -1.0F,
    0.0F,          0.0F,          (2 * kZCameraNear * kZCameraFar) / (kZCameraNear - kZCameraFar), 0.0F,
    // clang-format on
};

/** Generates and compiles shaders, and generates, compile and liks the program object. */
bool initProgram() {
    GLuint shaders[2] = {0};
    if (!(utils::createShaderFromString(shaders[0], GL_VERTEX_SHADER, kVertexShaderStr) &&
          utils::createShaderFromString(shaders[1], GL_FRAGMENT_SHADER, kFragmentShaderStr))) {
        fprintf(stderr, "Unable to create shaders from the given string.\n");
        return false;
    }

    if (!utils::createProgram(sGLProgram, shaders, 2)) {
        fprintf(stderr, "Unable to create program.\n");
        return false;
    }

    glDeleteShader(shaders[0]);
    glDeleteShader(shaders[1]);
    return true;
}

/** Initialize uniform input variables of the OpenGL program. */
bool initUniforms() {
    // Inefficient, but doesn't really matter in this case...
    if (!(utils::findAttribLocation(sGLProgram, sInPosLoc, "inPos", false) &&
          utils::findAttribLocation(sGLProgram, sInColLoc, "inCol", false) &&
          utils::findAttribLocation(sGLProgram, sPerspectiveMatLoc, "perspectiveMat", true) &&
          utils::findAttribLocation(sGLProgram, sCameraOffsetLoc, "cameraOffset", true))) {
        fprintf(stderr, "Unable to find attribute location.\n");
        return false;
    }

    glUseProgram(sGLProgram);
    glUniformMatrix4fv(static_cast<GLint>(sPerspectiveMatLoc), 1, GL_FALSE, sPerspectiveMat);
    glUniform2fv(static_cast<GLint>(sCameraOffsetLoc), 1, kCameraOffset);
    glUseProgram(0);

    return true;
}

/** Generate and initialize OpenGL buffer objects. */
void initBuffers() {
    glGenVertexArrays(1, &sVAO);
    glBindVertexArray(sVAO);

    glGenBuffers(1, &sVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBufferData(GL_ARRAY_BUFFER, kVertexDataSize, kInitialVertexData, GL_STATIC_DRAW);
}

/** Render to backbuffer */
void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(sGLProgram);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);

    glEnableVertexAttribArray(sInPosLoc);
    glEnableVertexAttribArray(sInColLoc);
    glVertexAttribPointer(sInPosLoc, kPositionDataPerVertex, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(
        sInColLoc,
        kColorDataPerVertex,
        GL_FLOAT,
        GL_FALSE,
        0,
        reinterpret_cast<GLvoid*>(kColorDataOffset));

    glDrawArrays(GL_TRIANGLES, 0, kNumVertices);

    glDisableVertexAttribArray(sInColLoc);
    glDisableVertexAttribArray(sInPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glUseProgram(0);
}

/** Delete OpenGL objects. */
void terminateRenderer() {
    fprintf(stderr, "Terminating renderer...\n");
    glDeleteVertexArrays(1, &sVAO);
    glDeleteBuffers(1, &sVBO);
    glDeleteProgram(sGLProgram);
}

/** Resize window respecting the aspect ratio. */
void resizeCallback(GLFWwindow* window, int width, int height) {
    sPerspectiveMat[0] = kFrustumScale * static_cast<float>(height) / static_cast<float>(width);
    glUseProgram(sGLProgram);
    glUniformMatrix4fv(static_cast<GLint>(sPerspectiveMatLoc), 1, GL_FALSE, sPerspectiveMat);
    glUseProgram(0);
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

int main() {
    GLFWwindow* window = utils::initGLFW("Rectangle 3D");
    utils::setGLFWCallbacks(window, utils::KEY_CALLBACK | utils::WINDOW_CLOSE_CALLBACK);
    glfwSetWindowSizeCallback(window, resizeCallback);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(utils::errorCallbackGL, nullptr);

    if (!initProgram()) {
        fprintf(stderr, "Unable to initialize the program.\n");
        utils::windowCloseCallbackGLFW(window);
        glfwTerminate();
        return -1;
    }
    initUniforms();
    initBuffers();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    terminateRenderer();
    glfwTerminate();

    return 0;
}
