#include "depch.h"
#include "Texture.h"
#include "DefinitelyEngine/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "DefinitelyEngine/Log.h";

namespace DefinitelyEngine {

    Texture* Texture::Create(const std::string& path) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::OpenGL: return new OpenGLTexture(path);
            default: DE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
        }
    }

}
