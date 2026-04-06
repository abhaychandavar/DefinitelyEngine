#pragma once

#include "DefinitelyEngine/Renderer/VertexArray.h"

namespace DefinitelyEngine {

    class OpenGLVertexArray : public VertexArray {
    public:
        OpenGLVertexArray();
        ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) override;
        virtual void SetIndexBuffer(IndexBuffer* indexBuffer) override;
        virtual IndexBuffer* GetIndexBuffer() const override { return m_IndexBuffer; }

    private:
        unsigned int m_VAO;
        VertexBuffer* m_VertexBuffer = nullptr;
        IndexBuffer* m_IndexBuffer = nullptr;
    };

}
