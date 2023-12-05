#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "GLFW/glfw3.h"
#include "glad/gl.h"

namespace utils {
    const char* readFileToBuffer(const char* path) {
        FILE* file = fopen(path, "rb");
        if (!file) {
            fprintf(stderr, "Couldn't open file %s.\n", path);
            return nullptr;
        }

        if (fseek(file, 0, SEEK_END) == -1) {
            fprintf(stderr, "Couldn't seek end of file.\n");
            return nullptr;
        }
        off_t fileSize = ftell(file);
        if (fileSize == -1) {
            fprintf(stderr, "Couldn't tell the size of the file.\n");
            return nullptr;
        }
        if (fseek(file, 0, SEEK_SET) == -1) {
            fprintf(stderr, "Couldn't seek start of file.\n");
            return nullptr;
        }

        size_t bufSize = static_cast<size_t>(fileSize);
        char* buf = new char[bufSize + 1];
        size_t readCount = fread(buf, 1, bufSize, file);
        if (ferror(file) != 0) {
            fprintf(stderr, "Couldn't read file.\n");
        } else {
            buf[readCount] = '\0';
        }
        fclose(file);

        return buf;
    }

    bool findAttribLocation(GLuint& program, GLuint& loc, const char* attribName, bool isUniform) {
        GLint iloc;
        if (isUniform) {
            iloc = glGetUniformLocation(program, attribName);
        } else {
            iloc = glGetAttribLocation(program, attribName);
        }

        if (iloc == -1) {
            fprintf(
                stderr,
                "Variable name %s does not correspond to an attribute in the program.\n",
                attribName);
            return false;
        }
        loc = static_cast<GLuint>(iloc);
        return true;
    }

    bool loadShader(GLuint& shader, const GLenum shaderType, const char* path) {
        const char* buf = readFileToBuffer(path);
        shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &buf, nullptr);
        delete[] buf;

        glCompileShader(shader);
        GLint compileStatus = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus == GL_FALSE) {
            GLint logLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

            GLchar* logBuffer = new GLchar[static_cast<size_t>(logLen)];
            glGetShaderInfoLog(shader, logLen, nullptr, logBuffer);
            const char* shaderTypeStr = nullptr;
            switch (shaderType) {
                case GL_VERTEX_SHADER: {
                    shaderTypeStr = "vertex";
                } break;
                case GL_FRAGMENT_SHADER: {
                    shaderTypeStr = "fragment";
                } break;
                default: {
                    shaderTypeStr = "unknown";
                }
            }

            fprintf(
                stderr,
                "OpenGL failed to compile %s shader (%s) due to: %s.\n",
                shaderTypeStr,
                path,
                logBuffer);

            delete[] logBuffer;
            glDeleteShader(shader);
            return false;
        }

        return true;
    }

    bool createShaderFromString(GLuint& shader, const GLenum shaderType, const char* shaderStr) {
        shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderStr, nullptr);
        glCompileShader(shader);

        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

            GLchar* infoLogStr = new GLchar[static_cast<size_t>(infoLogLen)];
            glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLogStr);

            const char* shaderTypeStr = nullptr;
            switch (shaderType) {
                case GL_VERTEX_SHADER: {
                    shaderTypeStr = "vertex";
                } break;
                case GL_FRAGMENT_SHADER: {
                    shaderTypeStr = "fragment";
                } break;
                default: {
                    shaderTypeStr = "unknown";
                }
            }

            fprintf(stderr, "OpenGL failed to compile %s shader: %s\n", shaderTypeStr, infoLogStr);
            delete[] infoLogStr;
            return false;
        }

        return true;
    }

    bool linkProgram(GLuint& program) {
        glLinkProgram(program);
        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (linked == GL_FALSE) {
            GLint logLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

            GLchar* logBuffer = new GLchar[static_cast<size_t>(logLen)];
            glGetProgramInfoLog(program, logLen, nullptr, logBuffer);
            fprintf(stderr, "OpenGL failed to link program due to: %s\n", logBuffer);
            delete[] logBuffer;
            glDeleteProgram(program);
            return false;
        }
        return true;
    }

    bool createProgram(GLuint& program, GLuint* shaders, const size_t numShaders) {
        program = glCreateProgram();
        for (size_t idx = 0; idx < numShaders; idx++) {
            glAttachShader(program, shaders[idx]);
        }
        linkProgram(program);
        for (size_t idx = 0; idx < numShaders; idx++) {
            glDetachShader(program, shaders[idx]);
        }
        return true;
    }

    void GLAPIENTRY errorCallbackGL(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam) {
        fprintf(
            stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type,
            severity,
            message);
    }

    void errorCallbackGLFW(int error, const char* desc) {
        fprintf(stderr, "GLFW error %d: %s\n", error, desc);
    }

    void keyCallbackGLFW(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void resizeCallbackGLFW(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void windowCloseCallbackGLFW(GLFWwindow* window) {
        fprintf(stderr, "Closing window...\n");
        glfwDestroyWindow(window);
    }

    void setGLFWCallbacks(GLFWwindow* window, const int flags) {
        if (flags & KEY_CALLBACK) {
            glfwSetKeyCallback(window, keyCallbackGLFW);
        }
        if (flags & RESIZE_CALLBACK) {
            glfwSetWindowSizeCallback(window, resizeCallbackGLFW);
        }
        if (flags & WINDOW_CLOSE_CALLBACK) {
            glfwSetWindowCloseCallback(window, windowCloseCallbackGLFW);
        }
    }

    GLFWwindow* initGLFW(const char* windowName) {
        if (!windowName) {
            fprintf(stderr, "initGLFW() requires a window name argument.\n");
            exit(-1);
        }

        printf("Initializing GLFW and creating window...\n");
        glfwSetErrorCallback(errorCallbackGLFW);
        if (glfwInit() == GLFW_FALSE) {
            fprintf(stderr, "GLFW failed to initialize...\n");
            exit(-1);
        }

        GLFWwindow* window = glfwCreateWindow(
            utils::kWindowWidth, utils::kWindowHeight, windowName, nullptr, nullptr);
        if (!window) {
            fprintf(stderr, "GLFW failed to create window...\n");
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(window);

        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            fprintf(stderr, "Glad failed to initialize OpenGL context\n");
            glfwTerminate();
            glfwDestroyWindow(window);
            exit(-1);
        }
        printf(
            "Loaded OpenGL version: %d.%d\n",
            GLAD_VERSION_MAJOR(version),
            GLAD_VERSION_MINOR(version));

        return window;
    }
}  // namespace utils
