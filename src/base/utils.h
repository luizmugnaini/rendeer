#ifndef RENDEER_UTILS_HEADER
#define RENDEER_UTILS_HEADER

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#define PI 3.14159F
#define Bit(x) (1 << x)

namespace utils {
    // GLFW window width.
    static const int kWindowWidth = 800;
    // GLFW window height.
    static const int kWindowHeight = 800;

    enum CallbackOptions {
        KEY_CALLBACK = Bit(0),
        RESIZE_CALLBACK = Bit(1),
        WINDOW_CLOSE_CALLBACK = Bit(2),
    };

    /**
     * @brief Reads the content from a file and writes it to a buffer.
     *
     * @param path Path to the file to be read.
     * @return Pointer to the buffer containing the content of the file. This pointer can be null if
     * the function was unable to read the contents.
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

    /** @brief Simple error callback function for OpenGL debugging. */
    void GLAPIENTRY errorCallbackGL(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam);

    /** @brief Report GLFW errors. */
    void errorCallbackGLFW(int error, const char* desc);

    /**
     * @brief If the use presses the escape key, the GLFW window is issued to close.
     */
    void keyCallbackGLFW(GLFWwindow* window, int key, int scancode, int action, int mods);

    /** @brief Makes a viewport transformation whenever the GLFW window is resized. */
    void resizeCallbackGLFW(GLFWwindow* window, int width, int height);

    /** @brief Clean up the OpenGL objects when closing the window, and destroy the window. */
    void windowCloseCallbackGLFW(GLFWwindow* window);

    /**
     * @brief Sets
     */
    void setGLFWCallbacks(GLFWwindow* window, int flags);

    /**
     * @brief Initialize GLFW.
     *
     * @param windowName Name to be given to the created window.
     * @return Pointer to the created GLFW window.
     */
    GLFWwindow* initGLFW(const char* windowName);
}  // namespace utils

#endif  // RENDEER_UTILS_HEADER
