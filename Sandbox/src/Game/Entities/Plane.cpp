#include "Plane.h"

Plane::Plane() {
    std::vector<float> positions = {
    // positions           // normals            // texCoords
    -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // 0
    -0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // 1
     0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // 2
     0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    DefinitelyEngine::VertexBuffer* vertexBuffer = DefinitelyEngine::VertexBuffer::Create();
    DefinitelyEngine::IndexBuffer* indexBuffer = DefinitelyEngine::IndexBuffer::Create();

    DefinitelyEngine::VertexBufferLayout layout;
    layout.Push(DefinitelyEngine::ShaderDataType::Float3, "a_Position");
    layout.Push(DefinitelyEngine::ShaderDataType::Float3, "a_Normal");
    layout.Push(DefinitelyEngine::ShaderDataType::Float2, "a_TexCoord");
    vertexBuffer->SetLayout(layout);

    m_VertexArray = DefinitelyEngine::VertexArray::Create();
    m_VertexArray->AddVertexBuffer(vertexBuffer);
    m_VertexArray->SetIndexBuffer(indexBuffer);

    vertexBuffer->SetData(&positions, (int)positions.size(), DefinitelyEngine::BufferUsage::Static);
    indexBuffer->SetData(indices, 6);

    m_shader = DefinitelyEngine::Shader::CreateFromFile("Shaders/Basic.shader");
    m_shader->Bind();

    this->r = 0.2f;
    this->g = 0.6f;
    this->b = 0.9f;

    m_shader->SetUniform4f("u_Color", std::array<float, 4>{this->r, this->g, this->b, 1.0f});

    DefinitelyEngine::Texture* tex = DefinitelyEngine::Texture::Create("Assets/Textures/DefinitelyEngine.png");
    tex->Bind();
    m_shader->SetUniform1i("u_Texture", 0);

    m_shader->Unbind();
    m_VertexArray->Unbind();
}

void Plane::Render(const glm::mat4& viewProjection, const glm::mat4& transform) {
    DrawRect(viewProjection, transform);
}

void Plane::DrawRect(const glm::mat4& viewProjection, const glm::mat4& transform) {
    r += 0.01f; if (r > 1.0f) r = 0.0f;
    g += 0.007f; if (g > 1.0f) g = 0.0f;
    b += 0.005f; if (b > 1.0f) b = 0.0f;

    m_shader->Bind();
    m_shader->SetUniformMat4("u_ViewProjection", viewProjection);
    m_shader->SetUniformMat4("u_Transform", transform);
    m_shader->SetUniform4f("u_Color", { r, g, b, 1.0f });
    m_VertexArray->Bind();

    DefinitelyEngine::RenderCommand::DrawIndexed(DefinitelyEngine::RendererAPI::PrimitiveType::Triangles, m_VertexArray->GetIndexBuffer()->GetCount());
}
