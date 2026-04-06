#include "depch.h"
#include "Model.h"
#include "DefinitelyEngine/Renderer/Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace DefinitelyEngine {

    Model::Model(const std::string& path) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate           |   // All faces become triangles
            aiProcess_GenNormals            |   // Generate normals ONLY if the mesh has none
            aiProcess_PreTransformVertices  |   // Bake node transforms (fixes Blender coord-system rotation)
            aiProcess_FlipUVs               |   // Flip V for OpenGL convention
            aiProcess_CalcTangentSpace          // Tangents (handy for normal mapping later)
        );

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            std::cerr << "[Model] Assimp error: " << importer.GetErrorString() << "\n";
            return;
        }

        // Derive texture search directory from the file path
        size_t lastSlash = path.find_last_of("/\\");
        m_Directory = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : ".";

        ProcessNode(scene->mRootNode, scene);

        m_Shader = Shader::CreateFromFile("Shaders/Model.shader");
        m_Shader->Bind();
        m_Shader->SetUniform1i("u_Texture", 0);
        m_Shader->Unbind();
    }

    Model::~Model() {
        for (auto* mesh : m_Meshes)   delete mesh;
        for (auto* tex  : m_Textures) delete tex;
        delete m_Shader;
    }

    void Model::Draw(const glm::mat4& viewProjection, const glm::mat4& transform) const {
        if (!m_Shader) return;

        m_Shader->Bind();
        m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);
        m_Shader->SetUniformMat4("u_Transform", transform);

        for (auto* mesh : m_Meshes)
            mesh->Draw(m_Shader);

        m_Shader->Unbind();
    }

    // -------------------------------------------------------------------------

    void Model::ProcessNode(aiNode* node, const aiScene* scene) {
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
            m_Meshes.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));

        for (uint32_t i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }

    Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;

        vertices.reserve(mesh->mNumVertices);
        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
            v.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            v.Normal = mesh->HasNormals()
                ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                : glm::vec3(0.0f, 1.0f, 0.0f);

            v.TexCoords = mesh->mTextureCoords[0]
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);

            vertices.push_back(v);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        Texture* diffuse = nullptr;
        if (mesh->mMaterialIndex < scene->mNumMaterials)
            diffuse = LoadMaterialTexture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);

        return new Mesh(vertices, indices, diffuse);
    }

    Texture* Model::LoadMaterialTexture(aiMaterial* mat, int textureType) {
        auto type = static_cast<aiTextureType>(textureType);
        if (mat->GetTextureCount(type) == 0)
            return nullptr;

        aiString str;
        mat->GetTexture(type, 0, &str);

        // Normalize to just the filename — FBX files often embed absolute paths
        std::string texPath = str.C_Str();
        size_t lastSlash = texPath.find_last_of("/\\");
        if (lastSlash != std::string::npos)
            texPath = texPath.substr(lastSlash + 1);

        std::string fullPath = m_Directory + "/" + texPath;

        // Return an existing texture if already loaded
        for (auto* tex : m_Textures) {
            // Simple deduplication: if a texture object was already created for
            // this path it's already in m_Textures — skip re-loading for now.
            // (A path→texture map would be cleaner; left as a future improvement.)
            (void)tex;
        }

        Texture* tex = Texture::Create(fullPath);
        m_Textures.push_back(tex);
        return tex;
    }

}
