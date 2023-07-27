#include "checkerror.h"

namespace Mandalin
{
    void CheckGLError(const char* file, int line)
    {
        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR) {
            const char* errorString;
            switch (error) {
            case GL_INVALID_ENUM:
                errorString = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorString = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorString = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                errorString = "GL_OUT_OF_MEMORY";
                break;
            case GL_STACK_UNDERFLOW:
                errorString = "GL_STACK_UNDERFLOW";
                break;
            case GL_STACK_OVERFLOW:
                errorString = "GL_STACK_OVERFLOW";
                break;
            default:
                errorString = "Unknown error";
                break;
            }

            std::cerr << "OpenGL error (" << errorString << ") at " << file << ":" << line << std::endl;
        }
    }
}