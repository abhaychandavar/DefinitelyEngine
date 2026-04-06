#include "SquareIndexBuffer.h"

SquareIndexBuffer::SquareIndexBuffer() {
    std::vector<float> positions = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
         -0.5f, 0.5f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    auto* vertexBuffer = DefinitelyEngine::VertexBuffer::Create();

    auto* indexBuffer = DefinitelyEngine::IndexBuffer::Create();

    DefinitelyEngine::VertexBufferLayout layout;
    layout.Push(DefinitelyEngine::ShaderDataType::Float2, "a_Position");
    vertexBuffer->SetLayout(layout);

    m_VertexArray = DefinitelyEngine::VertexArray::Create();
    m_VertexArray->AddVertexBuffer(vertexBuffer);
    m_VertexArray->SetIndexBuffer(indexBuffer);

    vertexBuffer->SetData(&positions, 4 * 2, DefinitelyEngine::BufferUsage::Static);
    indexBuffer->SetData(indices, 6);

    m_shader = DefinitelyEngine::Shader::CreateFromFile("Shaders/Basic.shader");
    m_shader->Bind();

    this->r = 0.0f;
    this->g = 1.0f;
    this->b = 0.0f;

    m_shader->SetUniform4f("u_Color", std::array<float, 4>{this->r, this->g, this->b, 1.0f});

    m_shader->Unbind();
    m_VertexArray->Unbind();
}

void SquareIndexBuffer::Render() {
    DrawTestSquare();
}

void SquareIndexBuffer::DrawTestSquare() {
    r += 0.01f; if (r > 1.0f) r = 0.0f;
    g += 0.007f; if (g > 1.0f) g = 0.0f;
    b += 0.005f; if (b > 1.0f) b = 0.0f;

    m_shader->Bind();
    m_shader->SetUniform4f("u_Color", { r, g, b, 1.0f });
    m_VertexArray->Bind();

    DefinitelyEngine::RenderCommand::DrawIndexed(DefinitelyEngine::RendererAPI::PrimitiveType::Triangles, m_VertexArray->GetIndexBuffer()->GetCount());
}
