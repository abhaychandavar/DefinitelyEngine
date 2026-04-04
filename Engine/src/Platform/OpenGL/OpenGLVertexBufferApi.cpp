#include "depch.h"
#include "Platform/OpenGL/OpenGLVertexBufferApi.h"
#include <glad/glad.h>

namespace DefinitelyEngine {
    OpenGLVertexBufferAPI::OpenGLVertexBufferAPI() {
        glGenBuffers(1, &this->m_BufferID);
    }

    OpenGLVertexBufferAPI::~OpenGLVertexBufferAPI() {
        
    }

    void OpenGLVertexBufferAPI::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, this->m_BufferID);
    }

    void OpenGLVertexBufferAPI::Unbind() const {

    }

    void OpenGLVertexBufferAPI::SetData(std::vector<float>* vertices, int size, DefinitelyEngine::BufferUsage bufferUsage) const {
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices->data(), this->BufferUsageToGLBufferUsage(bufferUsage));
    }

    void OpenGLVertexBufferAPI::SetLayout(int numOfComponents) const {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, numOfComponents, GL_FLOAT, GL_FALSE, sizeof(float) * numOfComponents, 0);
    }
}