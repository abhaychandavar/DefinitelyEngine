#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API Shader {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
        static Shader* CreateFromFile(const std::string& path);

        virtual void SetUniform4f(const std::string& name, std::array<float, 4> values) const = 0;
        virtual void SetUniform1i(const std::string& name, int value) const = 0;
        virtual void SetUniformMat4(const std::string& name, const glm::mat4& matrix) const = 0;
    };

}
