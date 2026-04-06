#include "SquareTextured.h"

SquareTextured::SquareTextured() {
    std::vector<float> positions = {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
         -0.5f, 0.5f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    auto* vertexBuffer = DefinitelyEngine::VertexBuffer::Create();

    auto* indexBuffer = DefinitelyEngine::IndexBuffer::Create();

    DefinitelyEngine::VertexBufferLayout layout;
    layout.Push(DefinitelyEngine::ShaderDataType::Float2, "a_Position");
    layout.Push(DefinitelyEngine::ShaderDataType::Float2, "a_TexCoord");
    vertexBuffer->SetLayout(layout);

    m_VertexArray = DefinitelyEngine::VertexArray::Create();
    m_VertexArray->AddVertexBuffer(vertexBuffer);
    m_VertexArray->SetIndexBuffer(indexBuffer);

    vertexBuffer->SetData(&positions, 4 * 4, DefinitelyEngine::BufferUsage::Static);
    indexBuffer->SetData(indices, 6);

    m_shader = DefinitelyEngine::Shader::CreateFromFile("Shaders/Basic.shader");
    m_shader->Bind();

    this->r = 0.0f;
    this->g = 1.0f;
    this->b = 0.0f;

    m_shader->SetUniform4f("u_Color", std::array<float, 4>{this->r, this->g, this->b, 1.0f});

    DefinitelyEngine::Texture* tex = DefinitelyEngine::Texture::Create("Assets/Textures/DefinitelyEngine.png");
    tex->Bind();
    m_shader->SetUniform1i("u_Texture", 0);

    m_shader->Unbind();
    m_VertexArray->Unbind();
}

void SquareTextured::Render() {
    DrawTestSquare();
}

void SquareTextured::DrawTestSquare() {
    r += 0.01f; if (r > 1.0f) r = 0.0f;
    g += 0.007f; if (g > 1.0f) g = 0.0f;
    b += 0.005f; if (b > 1.0f) b = 0.0f;

    m_shader->Bind();
    m_shader->SetUniform4f("u_Color", { r, g, b, 1.0f });
    m_VertexArray->Bind();

    DefinitelyEngine::RenderCommand::DrawIndexed(DefinitelyEngine::RendererAPI::PrimitiveType::Triangles, m_VertexArray->GetIndexBuffer()->GetCount());
}
