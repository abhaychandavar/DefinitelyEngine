#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "RendererAPI.h"

namespace DefinitelyEngine {

    enum class BufferUsage {
        Static,
        Dynamic,
        Stream
    };

    class DEFINITELY_ENGINE_API VertexBuffer {
        public:
            static VertexBuffer* Create();

            virtual void Bind() const = 0;
            virtual void Unbind() const = 0;
            virtual void SetData(std::vector<float>* verticies, int size, BufferUsage bufferUsage) const = 0;
            virtual void SetLayout(int numOfComponents) const = 0;
    };
}