#pragma once

#include "DefinitelyEngine/Log.h"
#include "DefinitelyEngine/Core.h"
#include <glad/glad.h>

namespace DefinitelyEngine {

    inline void GLAPIENTRY GLDebugCallback(
        GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei /*length*/,
        const GLchar* message, const void* /*userParam*/)
    {
        // Ignore noise: buffer detailed info, shader recompile notifications, etc.
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
            return;

        const char* srcStr = [source]() -> const char* {
            switch (source) {
                case GL_DEBUG_SOURCE_API:             return "API";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window System";
                case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
                case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third Party";
                case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
                default:                              return "Other";
            }
        }();

        const char* typeStr = [type]() -> const char* {
            switch (type) {
                case GL_DEBUG_TYPE_ERROR:               return "Error";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behaviour";
                case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
                case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
                case GL_DEBUG_TYPE_MARKER:              return "Marker";
                case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push Group";
                case GL_DEBUG_TYPE_POP_GROUP:           return "Pop Group";
                default:                               return "Other";
            }
        }();

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                DE_CORE_ERROR("[OpenGL] ({0}) {1} | Source: {2} | Type: {3}", id, message, srcStr, typeStr);
                DE_DEBUGBREAK();
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                DE_CORE_WARN("[OpenGL] ({0}) {1} | Source: {2} | Type: {3}", id, message, srcStr, typeStr);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                DE_CORE_INFO("[OpenGL] ({0}) {1} | Source: {2} | Type: {3}", id, message, srcStr, typeStr);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                DE_CORE_TRACE("[OpenGL] ({0}) {1} | Source: {2} | Type: {3}", id, message, srcStr, typeStr);
                break;
        }
    }

    inline void InitOpenGLDebug()
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugCallback, nullptr);
        // Suppress notification-level messages (driver info spam)
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        DE_CORE_INFO("[OpenGL] Debug output enabled");
    }

}
