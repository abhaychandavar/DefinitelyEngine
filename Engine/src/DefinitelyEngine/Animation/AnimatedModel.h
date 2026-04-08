#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "AnimatedMesh.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "Skeleton.h"
#include "DefinitelyEngine/Renderer/Shader.h"
#include "DefinitelyEngine/Texture/Texture.h"

// Forward-declare Assimp types to keep this header clean
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

#include <assimp/Importer.hpp>   // Importer must be a value member — it owns the aiScene*

namespace DefinitelyEngine {

    // AnimatedModel is a pure renderable: skinned geometry + skeleton data.
    // It does NOT own or drive an Animator. Game code creates an Animator
    // separately and passes its bone matrices into Draw each frame:
    //
    //   m_Animator->Update(dt);
    //   m_Model->Draw(vp, transform, m_Animator->GetBoneMatrices());
    //
    // This keeps the animation system independent of the rendering system.
    class DEFINITELY_ENGINE_API AnimatedModel {
    public:
        explicit AnimatedModel(const std::string& path);
        ~AnimatedModel();

        // Draw with externally-supplied bone matrices (from Animator::GetBoneMatrices()).
        void Draw(const glm::mat4& viewProjection,
                  const glm::mat4& transform,
                  const std::vector<glm::mat4>& boneMatrices) const;

        const Skeleton& GetSkeleton() const { return m_Skeleton; }

        // Returns the named clip, or nullptr if not found.
        // Logs available clip names when the name is not found.
        const AnimationClip* GetClip(const std::string& name) const;

    private:
        // Importer MUST be a value member — it owns the aiScene* lifetime.
        // Destroying the importer frees the scene; all aiNode/aiMesh pointers become dangling.
        Assimp::Importer                            m_Importer;

        std::vector<AnimatedMesh*>                  m_Meshes;
        std::vector<Texture*>                       m_Textures;   // owned; shared across meshes
        Shader*                                     m_Shader    = nullptr;
        std::string                                 m_Directory;

        Skeleton                                    m_Skeleton;
        std::unordered_map<std::string, AnimationClip> m_Clips;

        void          ProcessNode(aiNode* node, const aiScene* scene);
        AnimatedMesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
        void          ExtractBoneWeights(std::vector<AnimatedVertex>& vertices, aiMesh* mesh);
        void          BuildSkeleton(const aiNode* node, BoneNode& dst);
        void          LoadAnimations(const aiScene* scene);
        Texture*      LoadMaterialTexture(aiMaterial* mat, int textureType);

        static glm::mat4 AiToGlm(const aiMatrix4x4& m);
        static glm::vec3 AiToGlmVec3(const aiVector3D& v);
        static glm::quat AiToGlmQuat(const aiQuaternion& q);
    };

}
