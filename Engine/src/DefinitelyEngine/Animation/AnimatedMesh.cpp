#include "depch.h"
#include "AnimatedMesh.h"
#include "DefinitelyEngine/Renderer/VertexBuffer.h"
#include "DefinitelyEngine/Renderer/IndexBuffer.h"
#include "DefinitelyEngine/Renderer/VertexBufferLayout.h"
#include "DefinitelyEngine/Renderer/RenderCommand.h"
#include "DefinitelyEngine/Renderer/RendererAPI.h"

namespace DefinitelyEngine {

    AnimatedMesh::AnimatedMesh(const std::vector<AnimatedVertex>& vertices,
                               const std::vector<uint32_t>& indices,
                               Texture* diffuseTexture)
        : m_DiffuseTexture(diffuseTexture)
    {
        VertexBuffer* vb = VertexBuffer::Create();
        IndexBuffer*  ib = IndexBuffer::Create();

        // Layout must match attribute locations in Animated.shader:
        //   location 0 — a_Position   (Float3)
        //   location 1 — a_Normals    (Float3)
        //   location 2 — a_TexCoords  (Float2)
        //   location 3 — a_BoneIDs    (Int4)   → glVertexAttribIPointer
        //   location 4 — a_BoneWeights(Float4)
        VertexBufferLayout layout;
        layout.Push(ShaderDataType::Float3, "a_Position");
        layout.Push(ShaderDataType::Float3, "a_Normals");
        layout.Push(ShaderDataType::Float2, "a_TexCoords");
        layout.Push(ShaderDataType::Int4,   "a_BoneIDs");
        layout.Push(ShaderDataType::Float4, "a_BoneWeights");
        vb->SetLayout(layout);

        m_VertexArray = VertexArray::Create();
        m_VertexArray->AddVertexBuffer(vb);
        m_VertexArray->SetIndexBuffer(ib);

        // Upload raw struct bytes — uses the new void* overload on VertexBuffer.
        vb->SetData(vertices.data(),
                    (uint32_t)(vertices.size() * sizeof(AnimatedVertex)),
                    BufferUsage::Static);

        ib->SetData(const_cast<unsigned int*>(
                        reinterpret_cast<const unsigned int*>(indices.data())),
                    (unsigned int)indices.size());
    }

    AnimatedMesh::~AnimatedMesh() {
        delete m_VertexArray;
    }

    void AnimatedMesh::Draw(Shader* shader) const {
        if (m_DiffuseTexture) {
            m_DiffuseTexture->Bind(0);
            shader->SetUniform1i("u_HasTexture", 1);
        } else {
            shader->SetUniform1i("u_HasTexture", 0);
        }

        m_VertexArray->Bind();
        RenderCommand::DrawIndexed(RendererAPI::PrimitiveType::Triangles,
                                   m_VertexArray->GetIndexBuffer()->GetCount());
        m_VertexArray->Unbind();
    }

}
