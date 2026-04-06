#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API VertexArray {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) = 0;
        virtual void SetIndexBuffer(IndexBuffer* indexBuffer) = 0;
        virtual IndexBuffer* GetIndexBuffer() const = 0;

        static VertexArray* Create();
    };

}
