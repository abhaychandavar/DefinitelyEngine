#pragma once

#include "DefinitelyEngine/Renderer/Shader.h"

namespace DefinitelyEngine {

    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

    private:
        unsigned int CompileShader(unsigned int type, const std::string& source);

        unsigned int m_ProgramID;
    };

}
