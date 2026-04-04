#include "Triangle.h"

Triangle::Triangle() {
    std::vector<float> positions = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    m_Buffer = DefinitelyEngine::VertexBuffer::Create();
    m_Buffer->Bind();
    m_Buffer->SetData(&positions, 6, DefinitelyEngine::BufferUsage::Static);
    m_Buffer->SetLayout(2);

    

    m_shader = DefinitelyEngine::Shader::CreateFromFile("Shaders/Basic.shader");
    m_shader->Bind();
}

void Triangle::Render() {
    DrawTestTriangle();
}

void Triangle::DrawTestTriangle() {
    DefinitelyEngine::RenderCommand::DrawArrays(DefinitelyEngine::RendererAPI::PrimitiveType::Triangles, 3);
}
