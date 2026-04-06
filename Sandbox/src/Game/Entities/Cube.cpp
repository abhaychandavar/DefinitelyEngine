#include "Cube.h"

Cube::Cube() {
    std::vector<float> positions = {
    // positions           // normals            // texCoords

    // Front (0, 0, 1)
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // 0
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // 1
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // 2
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, // 3

    // Back (0, 0, -1)
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // 4
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // 5
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // 6
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // 7

    // Left (-1, 0, 0)
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // 8
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // 9
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, //10
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //11

    // Right (1, 0, 0)
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, //12
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, //13
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //14
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, //15

    // Top (0, 1, 0)
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, //16
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //17
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //18
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, //19

    // Bottom (0, -1, 0)
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, //20
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, //21
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, //22
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f  //23
};

unsigned int indices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,

    // Back
    4, 6, 5,
    6, 4, 7,

    // Left
    8, 9,10,
    10,11, 8,

    // Right
    12,14,13,
    14,12,15,

    // Top
    16,17,18,
    18,19,16,

    // Bottom
    20,22,21,
    22,20,23
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

    vertexBuffer->SetData(&positions, positions.size() * sizeof(float), DefinitelyEngine::BufferUsage::Static);
    indexBuffer->SetData(indices, 36);

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

void Cube::Render(const glm::mat4& viewProjection) {
    DrawTestSquare(viewProjection);
}

void Cube::DrawTestSquare(const glm::mat4& viewProjection) {
    r += 0.01f; if (r > 1.0f) r = 0.0f;
    g += 0.007f; if (g > 1.0f) g = 0.0f;
    b += 0.005f; if (b > 1.0f) b = 0.0f;

    m_shader->Bind();
    m_shader->SetUniformMat4("u_ViewProjection", viewProjection);
    m_shader->SetUniformMat4("u_Transform", glm::mat4(1.0f));
    m_shader->SetUniform4f("u_Color", { r, g, b, 1.0f });
    m_VertexArray->Bind();

    DefinitelyEngine::RenderCommand::DrawIndexed(DefinitelyEngine::RendererAPI::PrimitiveType::Triangles, m_VertexArray->GetIndexBuffer()->GetCount());
}
