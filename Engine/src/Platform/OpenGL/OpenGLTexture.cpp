#include "depch.h"
#include "OpenGLTexture.h"
#include "DefinitelyEngine/Log.h"

#include <glad/glad.h>
#include <StbImage/StbImage.h>

namespace DefinitelyEngine {

    OpenGLTexture::OpenGLTexture(const std::string& path)
        : m_FilePath(path), m_RendererID(0), m_Width(0), m_Height(0), m_BPP(0)
    {
        stbi_set_flip_vertically_on_load(1);
        unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
        if (!data) {
            DE_CORE_ERROR("Failed to load texture: {0}", path);
            return;
        }

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }

    OpenGLTexture::~OpenGLTexture() {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture::Bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGLTexture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}
