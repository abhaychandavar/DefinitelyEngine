#include "depch.h"
#include "Platform/OpenGL/OpenGLVertexBufferApi.h"
#include <glad/glad.h>

namespace DefinitelyEngine {
    OpenGLVertexBufferAPI::OpenGLVertexBufferAPI() {
        glGenBuffers(1, &this->m_BufferID);
    }

    OpenGLVertexBufferAPI::~OpenGLVertexBufferAPI() {
        glDeleteBuffers(1, &m_BufferID);
    }

    void OpenGLVertexBufferAPI::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, this->m_BufferID);
    }

    void OpenGLVertexBufferAPI::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBufferAPI::SetData(std::vector<float>* vertices, int size, DefinitelyEngine::BufferUsage bufferUsage) const {
        glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices->data(), this->BufferUsageToGLBufferUsage(bufferUsage));
    }
}