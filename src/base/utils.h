#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#define PI 3.14159F

// GLFW window width.
static const int kWindowWidth = 800;
// GLFW window height.
static const int kWindowHeight = 800;

/**
 * @brief Reads the content from a file and writes it to a buffer.
 *
 * @param path Path to the file to be read.
 * @return Pointer to the buffer containing the content of the file. This pointer can be null if the
 *         function was unable to read the contents.
 */
const char* readFileToBuffer(const char* path);

/**
 * @brief Computes the attribute location of an attribute.
 *
 * @param program Program object where the attribute will be searched.
 * @param loc Localization of the attribute.
 * @param attribName Name of the attribute.
 * @param isUniform Whether the attribute is a uniform variable or not.
 * @return True if the location was found successfully.
 */
bool findAttribLocation(GLuint& program, GLuint& loc, const char* attribName, bool isUniform);

/**
 * @brief Load shader into an OpenGL shader object.
 *
 * @param shader Reference to the shader object whose source will be created.
 * @param shaderType Type of the shader to be loaded.
 * @param path Path to shader source, relative to `graphics/src`.
 * @return True if the loading was successful, false otherwise.
 */
bool loadShader(GLuint& shader, const GLenum shaderType, const char* path);

/**
 * @brief Create shader object from a string.
 *
 * @param shader Reference to the shader object whose source will be created.
 * @param shaderType Type of the shader to be loaded.
 * @param shaderStr String representing the shader.
 * @return True if the loading was successful, false otherwise.
 */
bool createShaderFromString(GLuint& shader, const GLenum shaderType, const char* shaderStr);

/** @brief Conduces the linking of a given program object. */
bool linkProgram(GLuint& program);

/**
 * @brief Creates an OpenGL program object out of an array of shaders.
 *
 * @param program Reference to program object that will be created.
 * @param shaders Pointer to the array of shaders.
 * @param numShaders Number of shaders to be read from `shaders`.
 * @return True if the creation of the program was successful, false otherwise.
 */
bool createProgram(GLuint& program, GLuint* shaders, const size_t numShaders);

/** @brief Report GLFW errors. */
void errorCallback(int error, const char* desc);

/**
 * @brief Initialize GLFW.
 *
 * @param windowName Name to be given to the created window.
 * @return Pointer to the created GLFW window.
 */
GLFWwindow* initGLFW(const char* windowName);