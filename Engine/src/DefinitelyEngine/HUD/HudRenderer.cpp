#include "depch.h"
#include "HudRenderer.h"

#include "DefinitelyEngine/Renderer/IndexBuffer.h"
#include "DefinitelyEngine/Renderer/RenderCommand.h"
#include "DefinitelyEngine/Renderer/VertexBuffer.h"
#include "DefinitelyEngine/Renderer/VertexBufferLayout.h"

namespace {
    constexpr float kGlyphPixelHeight = 7.0f;
    constexpr float kGlyphAdvance = 6.0f;
}

namespace DefinitelyEngine {

    HudRenderer::HudRenderer()
        : m_Camera(0.0f, 1280.0f, 720.0f, 0.0f)
    {
        static const float quadVertices[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        static unsigned int quadIndices[] = {
            0, 1, 2,
            2, 3, 0
        };

        VertexBuffer* vertexBuffer = VertexBuffer::Create();
        IndexBuffer* indexBuffer = IndexBuffer::Create();

        VertexBufferLayout layout;
        layout.Push(ShaderDataType::Float2, "a_Position");
        vertexBuffer->SetLayout(layout);
        vertexBuffer->SetData(quadVertices, sizeof(quadVertices), BufferUsage::Static);

        indexBuffer->SetData(quadIndices, 6);

        m_QuadVertexArray = VertexArray::Create();
        m_QuadVertexArray->AddVertexBuffer(vertexBuffer);
        m_QuadVertexArray->SetIndexBuffer(indexBuffer);

        constexpr const char* vertexShader = R"(
#version 330 core
layout(location = 0) in vec2 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 0.0, 1.0);
}
)";

        constexpr const char* fragmentShader = R"(
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
    color = u_Color;
}
)";

        m_Shader = Shader::Create(vertexShader, fragmentShader);
    }

    HudRenderer::~HudRenderer()
    {
        delete m_Shader;
        delete m_QuadVertexArray;
    }

    void HudRenderer::BeginFrame(float width, float height)
    {
        m_FrameWidth = width;
        m_FrameHeight = height;
        m_Camera.SetBounds(0.0f, width, height, 0.0f);
        RenderCommand::SetDepthTestEnabled(false);
        m_FrameActive = true;
    }

    void HudRenderer::DrawQuad(const HudRect& rect, const std::array<float, 4>& color, HudAnchor anchor)
    {
        if (!m_FrameActive || !m_Shader || !m_QuadVertexArray)
            return;

        const HudRect resolved = ResolveRect(rect, anchor);
        const glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(resolved.x, resolved.y, 0.0f))
                                  * glm::scale(glm::mat4(1.0f), glm::vec3(resolved.width, resolved.height, 1.0f));

        m_Shader->Bind();
        m_Shader->SetUniformMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        m_Shader->SetUniformMat4("u_Transform", transform);
        m_Shader->SetUniform4f("u_Color", color);

        m_QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(RendererAPI::PrimitiveType::Triangles, m_QuadVertexArray->GetIndexBuffer()->GetCount());
    }

    void HudRenderer::DrawHealthBar(const HudRect& rect,
                                    float fill01,
                                    const std::array<float, 4>& backgroundColor,
                                    const std::array<float, 4>& fillColor,
                                    HudAnchor anchor)
    {
        const float clampedFill = glm::clamp(fill01, 0.0f, 1.0f);
        DrawQuad(rect, backgroundColor, anchor);

        HudRect fillRect = rect;
        fillRect.width *= clampedFill;
        DrawQuad(fillRect, fillColor, anchor);
    }

    void HudRenderer::DrawText(std::string_view text,
                               const HudVec2& position,
                               const HudTextStyle& style,
                               HudAnchor anchor)
    {
        if (!m_FrameActive)
            return;

        const float scale = glm::max(style.size, 1.0f);
        const float textWidth = MeasureTextWidth(text, scale);
        const float textHeight = kGlyphPixelHeight * scale;
        const HudVec2 resolvedPosition = ResolveTextPosition(position, textWidth, textHeight, anchor);

        float cursorX = resolvedPosition.x;
        for (char c : text) {
            DrawGlyph(c, cursorX, resolvedPosition.y, scale, style.color);
            cursorX += kGlyphAdvance * scale;
        }
    }

    void HudRenderer::EndFrame()
    {
        if (!m_FrameActive)
            return;

        RenderCommand::SetDepthTestEnabled(true);
        m_FrameActive = false;
    }

    HudRect HudRenderer::ResolveRect(const HudRect& rect, HudAnchor anchor) const
    {
        HudRect resolved = rect;

        switch (anchor) {
            case HudAnchor::TopLeft:
                break;
            case HudAnchor::TopRight:
                resolved.x = m_FrameWidth - rect.width - rect.x;
                break;
            case HudAnchor::BottomLeft:
                resolved.y = m_FrameHeight - rect.height - rect.y;
                break;
            case HudAnchor::BottomRight:
                resolved.x = m_FrameWidth - rect.width - rect.x;
                resolved.y = m_FrameHeight - rect.height - rect.y;
                break;
            case HudAnchor::Center:
                resolved.x = (m_FrameWidth - rect.width) * 0.5f + rect.x;
                resolved.y = (m_FrameHeight - rect.height) * 0.5f + rect.y;
                break;
        }

        return resolved;
    }

    HudVec2 HudRenderer::ResolveTextPosition(const HudVec2& position,
                                             float textWidth,
                                             float textHeight,
                                             HudAnchor anchor) const
    {
        HudVec2 resolved = position;

        switch (anchor) {
            case HudAnchor::TopLeft:
                break;
            case HudAnchor::TopRight:
                resolved.x = m_FrameWidth - textWidth - position.x;
                break;
            case HudAnchor::BottomLeft:
                resolved.y = m_FrameHeight - textHeight - position.y;
                break;
            case HudAnchor::BottomRight:
                resolved.x = m_FrameWidth - textWidth - position.x;
                resolved.y = m_FrameHeight - textHeight - position.y;
                break;
            case HudAnchor::Center:
                resolved.x = (m_FrameWidth - textWidth) * 0.5f + position.x;
                resolved.y = (m_FrameHeight - textHeight) * 0.5f + position.y;
                break;
        }

        return resolved;
    }

    void HudRenderer::DrawGlyph(char c, float x, float y, float scale, const std::array<float, 4>& color)
    {
        const std::array<unsigned char, 7> rows = GetGlyphRows(c);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                const unsigned char mask = static_cast<unsigned char>(1u << (4 - col));
                if ((rows[row] & mask) == 0)
                    continue;

                DrawQuad({ x + col * scale, y + row * scale, scale, scale }, color, HudAnchor::TopLeft);
            }
        }
    }

    std::array<unsigned char, 7> HudRenderer::GetGlyphRows(char c) const
    {
        switch (c) {
            case 'A': return { 0x04, 0x0A, 0x11, 0x11, 0x1F, 0x11, 0x11 };
            case 'C': return { 0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E };
            case 'D': return { 0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E };
            case 'E': return { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F };
            case 'I': return { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F };
            case 'L': return { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F };
            case 'M': return { 0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11 };
            case '0': return { 0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E };
            case '1': return { 0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E };
            case '2': return { 0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F };
            case '3': return { 0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E };
            case '4': return { 0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02 };
            case '5': return { 0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E };
            case '6': return { 0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E };
            case '7': return { 0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08 };
            case '8': return { 0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E };
            case '9': return { 0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E };
            case 'H': return { 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 };
            case 'N': return { 0x11, 0x19, 0x19, 0x15, 0x13, 0x13, 0x11 };
            case 'O': return { 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E };
            case 'P': return { 0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10 };
            case 'R': return { 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11 };
            case 'S': return { 0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E };
            case 'T': return { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 };
            case 'U': return { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E };
            case 'V': return { 0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04 };
            case 'W': return { 0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A };
            case 'X': return { 0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11 };
            case 'Y': return { 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04 };
            case '!': return { 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04 };
            case '.': return { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C };
            case '/': return { 0x01, 0x02, 0x02, 0x04, 0x08, 0x08, 0x10 };
            case ':': return { 0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00 };
            case ' ': return { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
            default:  return { 0x1F, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04 };
        }
    }

    float HudRenderer::MeasureTextWidth(std::string_view text, float scale) const
    {
        if (text.empty())
            return 0.0f;

        return (static_cast<float>(text.size()) * kGlyphAdvance - 1.0f) * scale;
    }

}
