#pragma once

#include "DefinitelyEngine/Renderer/Shader.h"
#include <unordered_map>

namespace DefinitelyEngine {

    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetUniform4f(const std::string& name, std::array<float, 4> values) const override;
        virtual void SetUniform1i(const std::string& name, int value) const override;
        virtual void SetUniformMat4(const std::string& name, const glm::mat4& matrix) const override;

    private:
        unsigned int CompileShader(unsigned int type, const std::string& source);
        int GetUniformLocation(const std::string& name) const;

        unsigned int m_ProgramID;
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    };

}
