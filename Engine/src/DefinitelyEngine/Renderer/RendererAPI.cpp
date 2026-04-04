#include "depch.h"
#include "DefinitelyEngine/Log.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace DefinitelyEngine {

    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    RendererAPI* RendererAPI::Create() {
        switch (s_API) {
            case API::OpenGL: return new OpenGLRendererAPI();
            default: DE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
        }
    }
}
