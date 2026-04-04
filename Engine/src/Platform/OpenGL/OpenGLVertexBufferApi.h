#pragma once
#include "DefinitelyEngine/Renderer/VertexBuffer.h"
#include "glad/glad.h"
#include "DefinitelyEngine/Log.h"

namespace DefinitelyEngine {

    class OpenGLVertexBufferAPI : public VertexBuffer {
        public:
            OpenGLVertexBufferAPI();
            ~OpenGLVertexBufferAPI();

            virtual void Bind() const override;
            virtual void Unbind() const override;
            virtual void SetData(std::vector<float>* verticies, int size, DefinitelyEngine::BufferUsage bufferUsage) const override;
            
            inline static GLenum BufferUsageToGLBufferUsage(BufferUsage bufferUsage) {
                switch (bufferUsage) {
                    case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
                    case BufferUsage::Static: return GL_STATIC_DRAW;
                    case BufferUsage::Stream: return GL_STREAM_DRAW;
                    default: DE_CORE_ASSERT(false, "Invalid buffer usage");
                }
            }

            virtual void OpenGLVertexBufferAPI::SetLayout(int numOfComponents) const override;
        
        private:
            unsigned int m_BufferID;
    };

}