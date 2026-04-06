#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "VertexArray.h"
#include "Shader.h"
#include "DefinitelyEngine/Texture/Texture.h"

namespace DefinitelyEngine {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class DEFINITELY_ENGINE_API Mesh {
    public:
        // diffuseTexture is non-owning; the Model that creates this Mesh owns the texture lifetime.
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Texture* diffuseTexture);
        ~Mesh();

        void Draw(Shader* shader) const;

    private:
        VertexArray* m_VertexArray = nullptr;
        Texture*     m_DiffuseTexture = nullptr; // non-owning
    };

}
