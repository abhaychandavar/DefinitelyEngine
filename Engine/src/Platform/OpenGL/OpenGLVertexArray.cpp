#include "depch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace DefinitelyEngine {

    OpenGLVertexArray::OpenGLVertexArray() {
        glGenVertexArrays(1, &m_VAO);
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &m_VAO);
    }

    void OpenGLVertexArray::Bind() const {
        glBindVertexArray(m_VAO);
    }

    void OpenGLVertexArray::Unbind() const {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(VertexBuffer* vertexBuffer) {
        glBindVertexArray(m_VAO);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        uint32_t index = 0;
        for (const auto& element : layout) {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index,
                element.GetComponentCount(),
                GL_FLOAT,
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)(uintptr_t)element.Offset
            );
            index++;
        }

        m_VertexBuffer = vertexBuffer;
    }

    void OpenGLVertexArray::SetIndexBuffer(IndexBuffer* indexBuffer) {
        glBindVertexArray(m_VAO);
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }

}
