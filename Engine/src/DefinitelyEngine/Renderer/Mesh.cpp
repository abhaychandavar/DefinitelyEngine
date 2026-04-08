#include "depch.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "RenderCommand.h"
#include "RendererAPI.h"

namespace DefinitelyEngine {

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Texture* diffuseTexture)
        : m_DiffuseTexture(diffuseTexture)
    {
        // Flatten Vertex structs into a plain float vector (required by VertexBuffer API)
        std::vector<float> flatData;
        flatData.reserve(vertices.size() * 8);
        for (const auto& v : vertices) {
            flatData.push_back(v.Position.x);
            flatData.push_back(v.Position.y);
            flatData.push_back(v.Position.z);
            flatData.push_back(v.Normal.x);
            flatData.push_back(v.Normal.y);
            flatData.push_back(v.Normal.z);
            flatData.push_back(v.TexCoords.x);
            flatData.push_back(v.TexCoords.y);
        }

        VertexBuffer* vb = VertexBuffer::Create();
        IndexBuffer*  ib = IndexBuffer::Create();

        VertexBufferLayout layout;
        layout.Push(ShaderDataType::Float3, "a_Position");
        layout.Push(ShaderDataType::Float3, "a_Normals");
        layout.Push(ShaderDataType::Float2, "a_TexCoords");
        vb->SetLayout(layout);

        m_VertexArray = VertexArray::Create();
        m_VertexArray->AddVertexBuffer(vb);
        m_VertexArray->SetIndexBuffer(ib);

        vb->SetData(&flatData, (int)flatData.size(), BufferUsage::Static);
        ib->SetData(const_cast<unsigned int*>(reinterpret_cast<const unsigned int*>(indices.data())),
                    (unsigned int)indices.size());
    }

    Mesh::~Mesh() {
        delete m_VertexArray;
    }

    void Mesh::Draw(Shader* shader) const {
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
