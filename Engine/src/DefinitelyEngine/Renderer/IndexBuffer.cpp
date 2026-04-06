#include "depch.h"
#include "IndexBuffer.h"
#include "DefinitelyEngine/Log.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLIndexBufferApi.h"

namespace DefinitelyEngine {

    IndexBuffer* IndexBuffer::Create() {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::OpenGL: return new OpenGLIndexBufferAPI();
            default: DE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
        }
    }

}
