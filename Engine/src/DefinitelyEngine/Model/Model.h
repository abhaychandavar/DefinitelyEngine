#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "DefinitelyEngine/Renderer/Mesh.h"
#include "DefinitelyEngine/Renderer/Shader.h"
#include "DefinitelyEngine/Texture/Texture.h"

// Forward-declare Assimp types to keep this header clean
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API Model {
    public:
        // path  - path to the model file (FBX, OBJ, GLTF, …)
        // Textures are expected to live next to the model file.
        explicit Model(const std::string& path);
        ~Model();

        // Draw all meshes with a given view-projection and optional transform.
        void Draw(const glm::mat4& viewProjection,
                  const glm::mat4& transform = glm::mat4(1.0f)) const;

    private:
        std::vector<Mesh*>    m_Meshes;
        std::vector<Texture*> m_Textures; // owned; shared across meshes that reuse the same image
        Shader*               m_Shader = nullptr;
        std::string           m_Directory;

        void     ProcessNode(aiNode* node, const aiScene* scene);
        Mesh*    ProcessMesh(aiMesh* mesh, const aiScene* scene);
        Texture* LoadMaterialTexture(aiMaterial* mat, int textureType);
    };

}
