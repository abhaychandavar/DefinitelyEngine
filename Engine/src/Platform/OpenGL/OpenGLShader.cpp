#include "depch.h"
#include "OpenGLShader.h"
#include "DefinitelyEngine/Log.h"

#include <glad/glad.h>

namespace DefinitelyEngine {

    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        m_ProgramID = glCreateProgram();
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

        glAttachShader(m_ProgramID, vs);
        glAttachShader(m_ProgramID, fs);
        glLinkProgram(m_ProgramID);
        glValidateProgram(m_ProgramID);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_ProgramID);
    }

    void OpenGLShader::Bind() const {
        glUseProgram(m_ProgramID);
    }

    void OpenGLShader::Unbind() const {
        glUseProgram(0);
    }

    unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source) {
        unsigned int id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            DE_CORE_ERROR("Shader compilation error: {0}", message);
            glDeleteShader(id);
            return 0;
        }
        return id;
    }

}
