#pragma once

#include "DefinitelyEngine/Renderer/IndexBuffer.h"

namespace DefinitelyEngine {

    class OpenGLIndexBufferAPI : public IndexBuffer {
    public:
        OpenGLIndexBufferAPI();
        ~OpenGLIndexBufferAPI();

        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void SetData(unsigned int* indices, unsigned int count) override;
        virtual unsigned int GetCount() const override { return m_Count; }

    private:
        unsigned int m_BufferID;
        unsigned int m_Count;
    };

}
