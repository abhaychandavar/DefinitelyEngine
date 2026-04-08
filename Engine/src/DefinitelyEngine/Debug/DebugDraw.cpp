#include "depch.h"
#include "DebugDraw.h"
#include "DefinitelyEngine/Renderer/VertexBufferLayout.h"
#include "DefinitelyEngine/Renderer/RenderCommand.h"
#include "DefinitelyEngine/Renderer/RendererAPI.h"

namespace DefinitelyEngine {

    DebugDraw::DebugDraw() {
        m_Shader = Shader::CreateFromFile("Shaders/DebugLine.shader");

        VertexBufferLayout layout;
        layout.Push(ShaderDataType::Float3, "a_Position");
        layout.Push(ShaderDataType::Float4, "a_Color");

        m_VertexBuffer = VertexBuffer::Create();
        m_VertexBuffer->SetLayout(layout);

        m_VertexArray = VertexArray::Create();
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->Unbind();
    }

    DebugDraw::~DebugDraw() {
        delete m_Shader;
        delete m_VertexArray;
        // VertexBuffer is owned by VertexArray
    }

    void DebugDraw::DrawLine(const glm::vec3& a, const glm::vec3& b, const glm::vec4& color) {
        m_Buffer.push_back({ a, color });
        m_Buffer.push_back({ b, color });
    }

    void DebugDraw::DrawCircle(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV, float radius,
                                const glm::vec4& color, int segments) {
        float step = glm::two_pi<float>() / (float)segments;
        for (int i = 0; i < segments; i++) {
            float a0 = step * (float)i;
            float a1 = step * (float)(i + 1);
            glm::vec3 p0 = center + axisU * (radius * glm::cos(a0)) + axisV * (radius * glm::sin(a0));
            glm::vec3 p1 = center + axisU * (radius * glm::cos(a1)) + axisV * (radius * glm::sin(a1));
            DrawLine(p0, p1, color);
        }
    }

    void DebugDraw::DrawArc(const glm::vec3& center, const glm::vec3& axisU,
                             const glm::vec3& axisV, float radius,
                             float startAngle, float endAngle,
                             const glm::vec4& color, int segments) {
        float step = (endAngle - startAngle) / (float)segments;
        for (int i = 0; i < segments; i++) {
            float a0 = startAngle + step * (float)i;
            float a1 = startAngle + step * (float)(i + 1);
            glm::vec3 p0 = center + axisU * (radius * glm::cos(a0)) + axisV * (radius * glm::sin(a0));
            glm::vec3 p1 = center + axisU * (radius * glm::cos(a1)) + axisV * (radius * glm::sin(a1));
            DrawLine(p0, p1, color);
        }
    }

    void DebugDraw::DrawBox(const glm::vec3& center, const glm::vec3& h, const glm::vec4& color) {
        // 8 corners
        glm::vec3 c[8] = {
            center + glm::vec3(-h.x, -h.y, -h.z),
            center + glm::vec3( h.x, -h.y, -h.z),
            center + glm::vec3( h.x, -h.y,  h.z),
            center + glm::vec3(-h.x, -h.y,  h.z),
            center + glm::vec3(-h.x,  h.y, -h.z),
            center + glm::vec3( h.x,  h.y, -h.z),
            center + glm::vec3( h.x,  h.y,  h.z),
            center + glm::vec3(-h.x,  h.y,  h.z),
        };
        // Bottom ring
        DrawLine(c[0], c[1], color); DrawLine(c[1], c[2], color);
        DrawLine(c[2], c[3], color); DrawLine(c[3], c[0], color);
        // Top ring
        DrawLine(c[4], c[5], color); DrawLine(c[5], c[6], color);
        DrawLine(c[6], c[7], color); DrawLine(c[7], c[4], color);
        // Vertical pillars
        DrawLine(c[0], c[4], color); DrawLine(c[1], c[5], color);
        DrawLine(c[2], c[6], color); DrawLine(c[3], c[7], color);
    }

    void DebugDraw::DrawCapsule(const glm::vec3& bottomCenter, float radius, float height,
                                 const glm::vec4& color, int segments) {
        glm::vec3 topCenter = bottomCenter + glm::vec3(0.0f, height, 0.0f);

        // Cylinder rings
        DrawCircle(bottomCenter, { 1,0,0 }, { 0,0,1 }, radius, color, segments);
        DrawCircle(topCenter,    { 1,0,0 }, { 0,0,1 }, radius, color, segments);

        // 4 vertical side lines
        DrawLine(bottomCenter + glm::vec3( radius, 0, 0), topCenter + glm::vec3( radius, 0, 0), color);
        DrawLine(bottomCenter + glm::vec3(-radius, 0, 0), topCenter + glm::vec3(-radius, 0, 0), color);
        DrawLine(bottomCenter + glm::vec3(0, 0,  radius), topCenter + glm::vec3(0, 0,  radius), color);
        DrawLine(bottomCenter + glm::vec3(0, 0, -radius), topCenter + glm::vec3(0, 0, -radius), color);

        // Bottom cap: two semicircle arcs curving downward (π → 2π)
        DrawArc(bottomCenter, { 1,0,0 }, { 0,-1,0 }, radius,
                0.0f, glm::pi<float>(), color, segments / 2);
        DrawArc(bottomCenter, { 0,0,1 }, { 0,-1,0 }, radius,
                0.0f, glm::pi<float>(), color, segments / 2);

        // Top cap: two semicircle arcs curving upward (0 → π)
        DrawArc(topCenter, { 1,0,0 }, { 0,1,0 }, radius,
                0.0f, glm::pi<float>(), color, segments / 2);
        DrawArc(topCenter, { 0,0,1 }, { 0,1,0 }, radius,
                0.0f, glm::pi<float>(), color, segments / 2);
    }

    void DebugDraw::Flush(const glm::mat4& viewProjection) {
        if (m_Buffer.empty()) return;

        m_VertexBuffer->SetData(m_Buffer.data(),
                                (uint32_t)(m_Buffer.size() * sizeof(LineVertex)),
                                BufferUsage::Stream);

        m_Shader->Bind();
        m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);
        m_VertexArray->Bind();

        RenderCommand::DrawArrays(RendererAPI::PrimitiveType::Lines, (int)m_Buffer.size());

        m_VertexArray->Unbind();
        m_Shader->Unbind();
        m_Buffer.clear();
    }

}
