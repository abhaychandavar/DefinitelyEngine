#include "depch.h"
#include "VertexBuffer.h"
#include "DefinitelyEngine/Log.h"
#include "Platform/OpenGL/OpenGLVertexBufferApi.h"

namespace DefinitelyEngine {
     VertexBuffer* VertexBuffer::Create() {
          switch (RendererAPI::GetAPI()) {
               case RendererAPI::API::OpenGL: return new OpenGLVertexBufferAPI();
               default: DE_CORE_ASSERT(false, "Unknown API"); return nullptr;
          }
     }
}