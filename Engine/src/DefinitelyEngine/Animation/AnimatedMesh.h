#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "DefinitelyEngine/Renderer/VertexArray.h"
#include "DefinitelyEngine/Renderer/Shader.h"
#include "DefinitelyEngine/Texture/Texture.h"

namespace DefinitelyEngine {

    // Vertex layout for a skinned mesh.
    // BoneIDs are initialised to 0 (not -1) so the shader never indexes out of bounds.
    // BoneWeights are initialised to 0.0f; any vertex with no influences gets
    // BoneWeights[0] = 1.0f (fully bound to bone 0) so the skin matrix is never zero.
    struct AnimatedVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        int       BoneIDs[4];      // indices into u_BoneMatrices[]
        float     BoneWeights[4];  // corresponding blend weights (sum ≈ 1.0)
    };

    class DEFINITELY_ENGINE_API AnimatedMesh {
    public:
        // diffuseTexture is non-owning; AnimatedModel owns the texture lifetime.
        AnimatedMesh(const std::vector<AnimatedVertex>& vertices,
                     const std::vector<uint32_t>& indices,
                     Texture* diffuseTexture);
        ~AnimatedMesh();

        void Draw(Shader* shader) const;

    private:
        VertexArray* m_VertexArray    = nullptr;
        Texture*     m_DiffuseTexture = nullptr;  // non-owning
    };

}
