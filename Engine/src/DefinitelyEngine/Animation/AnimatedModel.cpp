#include "depch.h"
#include "AnimatedModel.h"
#include "DefinitelyEngine/Log.h"
#include "DefinitelyEngine/Renderer/Shader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace DefinitelyEngine {

    AnimatedModel::AnimatedModel(const std::string& path) {
        // NOTE: aiProcess_PreTransformVertices is intentionally omitted —
        // it bakes all node transforms into vertex positions, which destroys
        // the bone hierarchy needed for skeletal animation.
        const aiScene* scene = m_Importer.ReadFile(path,
            aiProcess_Triangulate       |
            aiProcess_GenNormals        |
            aiProcess_FlipUVs           |
            aiProcess_CalcTangentSpace
        );

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            DE_CORE_ERROR("[AnimatedModel] Assimp error: {0}", m_Importer.GetErrorString());
            return;
        }

        // Global inverse transform: converts from scene-root space back to model space.
        // Every final bone matrix is premultiplied by this.
        m_Skeleton.globalInverseTransform = glm::inverse(AiToGlm(scene->mRootNode->mTransformation));

        size_t lastSlash = path.find_last_of("/\\");
        m_Directory = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : ".";

        // Build geometry + populate m_Skeleton.boneInfoMap / boneCount
        ProcessNode(scene->mRootNode, scene);

        // Build the bone hierarchy tree (used by Animator for traversal)
        BuildSkeleton(scene->mRootNode, m_Skeleton.rootNode);

        // Load animation clips
        LoadAnimations(scene);

        m_Shader = Shader::CreateFromFile("Shaders/Animated.shader");
        m_Shader->Bind();
        m_Shader->SetUniform1i("u_Texture", 0);
        m_Shader->Unbind();

        DE_CORE_INFO("[AnimatedModel] Loaded '{0}': {1} meshes, {2} bones, {3} clips",
                     path, m_Meshes.size(), m_Skeleton.boneCount, m_Clips.size());
    }

    AnimatedModel::~AnimatedModel() {
        for (auto* mesh : m_Meshes)   delete mesh;
        for (auto* tex  : m_Textures) delete tex;
        delete m_Shader;
    }

    void AnimatedModel::Draw(const glm::mat4& viewProjection,
                             const glm::mat4& transform,
                             const std::vector<glm::mat4>& boneMatrices) const {
        if (!m_Shader) return;

        m_Shader->Bind();
        m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);
        m_Shader->SetUniformMat4("u_Transform", transform);

        if (!boneMatrices.empty()) {
            m_Shader->SetUniformMat4Array("u_BoneMatrices", boneMatrices);
        } else {
            // Safety fallback: upload identity matrices so the shader never reads garbage.
            static const std::vector<glm::mat4> s_Identity(100, glm::mat4(1.0f));
            m_Shader->SetUniformMat4Array("u_BoneMatrices", s_Identity);
        }

        for (auto* mesh : m_Meshes)
            mesh->Draw(m_Shader);

        m_Shader->Unbind();
    }

    const AnimationClip* AnimatedModel::GetClip(const std::string& name) const {
        auto it = m_Clips.find(name);
        if (it != m_Clips.end()) return &it->second;

        DE_CORE_WARN("[AnimatedModel] Clip '{0}' not found. Available clips:", name);
        for (const auto& [clipName, _] : m_Clips)
            DE_CORE_WARN("  - '{0}'", clipName);
        return nullptr;
    }

    // -------------------------------------------------------------------------

    void AnimatedModel::ProcessNode(aiNode* node, const aiScene* scene) {
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
            m_Meshes.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));

        for (uint32_t i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }

    AnimatedMesh* AnimatedModel::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<AnimatedVertex> vertices;
        std::vector<uint32_t>       indices;

        vertices.reserve(mesh->mNumVertices);
        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            AnimatedVertex v;
            v.Position  = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            v.Normal    = mesh->HasNormals()
                ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                : glm::vec3(0.0f, 1.0f, 0.0f);
            v.TexCoords = mesh->mTextureCoords[0]
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);

            // Default: no bone influence. BoneIDs=0 so the shader never reads [-1].
            // BoneWeights=0 — ExtractBoneWeights will set the real influences below.
            v.BoneIDs[0] = v.BoneIDs[1] = v.BoneIDs[2] = v.BoneIDs[3] = 0;
            v.BoneWeights[0] = v.BoneWeights[1] = v.BoneWeights[2] = v.BoneWeights[3] = 0.0f;

            vertices.push_back(v);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Fill bone IDs and weights into the vertex array
        ExtractBoneWeights(vertices, mesh);

        Texture* diffuse = nullptr;
        if (mesh->mMaterialIndex < scene->mNumMaterials)
            diffuse = LoadMaterialTexture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);

        return new AnimatedMesh(vertices, indices, diffuse);
    }

    void AnimatedModel::ExtractBoneWeights(std::vector<AnimatedVertex>& vertices, aiMesh* mesh) {
        for (uint32_t b = 0; b < mesh->mNumBones; b++) {
            aiBone* bone     = mesh->mBones[b];
            std::string name = bone->mName.C_Str();

            // Register bone if we haven't seen it yet
            if (m_Skeleton.boneInfoMap.find(name) == m_Skeleton.boneInfoMap.end()) {
                BoneInfo info;
                info.id           = m_Skeleton.boneCount++;
                info.offsetMatrix = AiToGlm(bone->mOffsetMatrix);
                m_Skeleton.boneInfoMap[name] = info;
            }

            int boneId = m_Skeleton.boneInfoMap[name].id;

            for (uint32_t w = 0; w < bone->mNumWeights; w++) {
                uint32_t vertexId = bone->mWeights[w].mVertexId;
                float    weight   = bone->mWeights[w].mWeight;

                // Fill the first free slot (up to 4 influences per vertex)
                for (int slot = 0; slot < 4; slot++) {
                    if (vertices[vertexId].BoneWeights[slot] == 0.0f) {
                        vertices[vertexId].BoneIDs[slot]     = boneId;
                        vertices[vertexId].BoneWeights[slot] = weight;
                        break;
                    }
                }
            }
        }

        // Post-pass: any vertex with no influences gets full weight on bone 0.
        // This prevents the skin matrix from being a zero matrix in the shader.
        for (auto& v : vertices) {
            bool hasInfluence = false;
            for (int slot = 0; slot < 4; slot++) {
                if (v.BoneWeights[slot] > 0.0f) { hasInfluence = true; break; }
            }
            if (!hasInfluence) {
                v.BoneIDs[0]     = 0;
                v.BoneWeights[0] = 1.0f;
            }
        }
    }

    void AnimatedModel::BuildSkeleton(const aiNode* node, BoneNode& dst) {
        dst.name           = node->mName.C_Str();
        dst.localTransform = AiToGlm(node->mTransformation);
        dst.children.resize(node->mNumChildren);
        for (uint32_t i = 0; i < node->mNumChildren; i++)
            BuildSkeleton(node->mChildren[i], dst.children[i]);
    }

    void AnimatedModel::LoadAnimations(const aiScene* scene) {
        for (uint32_t a = 0; a < scene->mNumAnimations; a++) {
            aiAnimation* anim = scene->mAnimations[a];

            AnimationClip clip;
            clip.name           = anim->mName.C_Str();
            clip.durationTicks  = (float)anim->mDuration;
            clip.ticksPerSecond = (anim->mTicksPerSecond > 0.0)
                                    ? (float)anim->mTicksPerSecond
                                    : 25.0f;

            for (uint32_t c = 0; c < anim->mNumChannels; c++) {
                aiNodeAnim* ch = anim->mChannels[c];
                BoneChannel channel;
                channel.boneName = ch->mNodeName.C_Str();

                for (uint32_t k = 0; k < ch->mNumPositionKeys; k++)
                    channel.posKeys.push_back({ (float)ch->mPositionKeys[k].mTime,
                                                AiToGlmVec3(ch->mPositionKeys[k].mValue) });

                for (uint32_t k = 0; k < ch->mNumRotationKeys; k++)
                    channel.rotKeys.push_back({ (float)ch->mRotationKeys[k].mTime,
                                                AiToGlmQuat(ch->mRotationKeys[k].mValue) });

                for (uint32_t k = 0; k < ch->mNumScalingKeys; k++)
                    channel.scaleKeys.push_back({ (float)ch->mScalingKeys[k].mTime,
                                                  AiToGlmVec3(ch->mScalingKeys[k].mValue) });

                clip.channelMap[channel.boneName] = (int)clip.channels.size();
                clip.channels.push_back(std::move(channel));
            }

            DE_CORE_INFO("[AnimatedModel] Clip '{0}' — {1:.1f} ticks @ {2:.1f} tps",
                         clip.name, clip.durationTicks, clip.ticksPerSecond);

            m_Clips[clip.name] = std::move(clip);
        }
    }

    Texture* AnimatedModel::LoadMaterialTexture(aiMaterial* mat, int textureType) {
        auto type = static_cast<aiTextureType>(textureType);
        if (mat->GetTextureCount(type) == 0) return nullptr;

        aiString str;
        mat->GetTexture(type, 0, &str);

        std::string texPath = str.C_Str();
        size_t lastSlash = texPath.find_last_of("/\\");
        if (lastSlash != std::string::npos) texPath = texPath.substr(lastSlash + 1);

        Texture* tex = Texture::Create(m_Directory + "/" + texPath);
        m_Textures.push_back(tex);
        return tex;
    }

    // -------------------------------------------------------------------------
    // Assimp → GLM conversion helpers

    glm::mat4 AnimatedModel::AiToGlm(const aiMatrix4x4& m) {
        // Assimp is row-major; GLM is column-major — transpose to convert.
        return glm::transpose(glm::make_mat4(&m.a1));
    }

    glm::vec3 AnimatedModel::AiToGlmVec3(const aiVector3D& v) {
        return { v.x, v.y, v.z };
    }

    glm::quat AnimatedModel::AiToGlmQuat(const aiQuaternion& q) {
        // GLM quat constructor is (w, x, y, z); Assimp stores (x, y, z, w).
        return glm::quat(q.w, q.x, q.y, q.z);
    }

}
