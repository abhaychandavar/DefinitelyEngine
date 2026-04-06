#pragma once

#include "DefinitelyEngine/Texture/Texture.h"

namespace DefinitelyEngine {

    class OpenGLTexture : public Texture {
    public:
        OpenGLTexture(const std::string& path);
        ~OpenGLTexture();

        virtual void Bind(unsigned int slot = 0) const override;
        virtual void Unbind() const override;

        virtual int GetWidth() const override { return m_Width; }
        virtual int GetHeight() const override { return m_Height; }

    private:
        unsigned int m_RendererID;
        std::string m_FilePath;
        int m_Width, m_Height, m_BPP;
    };

}
