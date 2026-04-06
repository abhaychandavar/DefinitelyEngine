#include "Square.h"

Square::Square() {
    std::vector<float> positions = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,

         0.5f,  0.5f,
         -0.5f, 0.5f,
         -0.5f, -0.5f
    };

    DefinitelyEngine::VertexBufferLayout layout;
    layout.Push(DefinitelyEngine::ShaderDataType::Float2, "a_Position");

    m_Buffer = DefinitelyEngine::VertexBuffer::Create();
    m_Buffer->SetLayout(layout);
    m_Buffer->SetData(&positions, 6 * 2, DefinitelyEngine::BufferUsage::Static);

    m_VertexArray = DefinitelyEngine::VertexArray::Create();
    m_VertexArray->AddVertexBuffer(m_Buffer);

    

    m_shader = DefinitelyEngine::Shader::CreateFromFile("Shaders/Basic.shader");
    m_shader->Bind();
}

void Square::Render() {
    DrawTestSquare();
}

void Square::DrawTestSquare() {
    m_VertexArray->Bind();
    DefinitelyEngine::RenderCommand::DrawArrays(DefinitelyEngine::RendererAPI::PrimitiveType::Triangles, 6);
}
