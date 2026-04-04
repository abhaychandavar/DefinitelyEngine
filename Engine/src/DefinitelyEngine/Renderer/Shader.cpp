#include "depch.h"
#include "Shader.h"
#include "DefinitelyEngine/Log.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>
#include <sstream>

namespace DefinitelyEngine {

    Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
            default: DE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
        }
    }

    Shader* Shader::CreateFromFile(const std::string& path) {
        std::ifstream file(path);
        DE_CORE_ASSERT(file.is_open(), "Failed to open shader file: {0}", path);

        std::stringstream sources[2];
        int current = -1;

        std::string line;
        while (std::getline(file, line)) {
            if (line.find("#shader vertex") != std::string::npos)
                current = 0;
            else if (line.find("#shader fragment") != std::string::npos)
                current = 1;
            else if (current != -1)
                sources[current] << line << '\n';
        }

        return Create(sources[0].str(), sources[1].str());
    }

}
