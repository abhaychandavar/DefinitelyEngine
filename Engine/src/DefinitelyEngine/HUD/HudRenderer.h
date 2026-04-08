#pragma once

#include "DefinitelyEngine/Core.h"
#include "DefinitelyEngine/Camera/OrthographicCamera.h"
#include "DefinitelyEngine/HUD/HudTypes.h"
#include "DefinitelyEngine/Renderer/Shader.h"
#include "DefinitelyEngine/Renderer/VertexArray.h"

#include <array>
#include <string_view>

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API HudRenderer {
    public:
        HudRenderer();
        ~HudRenderer();

        void BeginFrame(float width, float height);
        void DrawQuad(const HudRect& rect, const std::array<float, 4>& color, HudAnchor anchor = HudAnchor::TopLeft);
        void DrawHealthBar(const HudRect& rect,
                           float fill01,
                           const std::array<float, 4>& backgroundColor,
                           const std::array<float, 4>& fillColor,
                           HudAnchor anchor = HudAnchor::TopLeft);
        void DrawText(std::string_view text,
                      const HudVec2& position,
                      const HudTextStyle& style,
                      HudAnchor anchor = HudAnchor::TopLeft);
        void EndFrame();

    private:
        HudRect ResolveRect(const HudRect& rect, HudAnchor anchor) const;
        HudVec2 ResolveTextPosition(const HudVec2& position,
                                    float textWidth,
                                    float textHeight,
                                    HudAnchor anchor) const;
        void DrawGlyph(char c, float x, float y, float scale, const std::array<float, 4>& color);
        std::array<unsigned char, 7> GetGlyphRows(char c) const;
        float MeasureTextWidth(std::string_view text, float scale) const;

    private:
        OrthographicCamera m_Camera;
        VertexArray* m_QuadVertexArray = nullptr;
        Shader* m_Shader = nullptr;
        float m_FrameWidth = 0.0f;
        float m_FrameHeight = 0.0f;
        bool m_FrameActive = false;
    };

}
