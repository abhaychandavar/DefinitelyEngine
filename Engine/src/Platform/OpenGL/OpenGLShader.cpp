#include "depch.h"
#include "OpenGLShader.h"
#include "DefinitelyEngine/Log.h"
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

    int OpenGLShader::GetUniformLocation(const std::string& name) const {
        auto it = m_UniformLocationCache.find(name);
        if (it != m_UniformLocationCache.end())
            return it->second;

        int loc = glGetUniformLocation(m_ProgramID, name.c_str());
        if (loc == -1)
            DE_CORE_WARN("Uniform '{0}' not found in shader", name);

        m_UniformLocationCache[name] = loc;
        return loc;
    }

    void OpenGLShader::SetUniform4f(const std::string& name, std::array<float, 4> values) const {
        int loc = GetUniformLocation(name);
        if (loc == -1) return;
        glUniform4f(loc, values[0], values[1], values[2], values[3]);
    }

    void OpenGLShader::SetUniform1i(const std::string& name, int value) const {
        int loc = GetUniformLocation(name);
        if (loc == -1) return;
        glUniform1i(loc, value);
    }

    void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) const {
        int loc = GetUniformLocation(name);
        if (loc == -1) return;
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::SetUniformMat4Array(const std::string& name, const std::vector<glm::mat4>& matrices) const {
        if (matrices.empty()) return;
        int loc = GetUniformLocation(name);
        if (loc == -1) return;
        glUniformMatrix4fv(loc, (GLsizei)matrices.size(), GL_FALSE, glm::value_ptr(matrices[0]));
    }
}
