#include "depch.h"
#include "VertexArray.h"
#include "DefinitelyEngine/Log.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace DefinitelyEngine {

    VertexArray* VertexArray::Create() {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
            default: DE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
        }
    }

}
