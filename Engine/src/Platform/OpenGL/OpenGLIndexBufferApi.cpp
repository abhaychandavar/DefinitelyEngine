#include "depch.h"
#include "OpenGLIndexBufferApi.h"

#include <glad/glad.h>

namespace DefinitelyEngine {

    OpenGLIndexBufferAPI::OpenGLIndexBufferAPI()
        : m_Count(0) {
        glGenBuffers(1, &m_BufferID);
    }

    OpenGLIndexBufferAPI::~OpenGLIndexBufferAPI() {
        glDeleteBuffers(1, &m_BufferID);
    }

    void OpenGLIndexBufferAPI::Bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    }

    void OpenGLIndexBufferAPI::Unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void OpenGLIndexBufferAPI::SetData(unsigned int* indices, unsigned int count) {
        m_Count = count;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    }

}
