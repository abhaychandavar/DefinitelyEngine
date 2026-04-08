#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "DefinitelyEngine/Renderer/Shader.h"
#include "DefinitelyEngine/Renderer/VertexArray.h"
#include "DefinitelyEngine/Renderer/VertexBuffer.h"

namespace DefinitelyEngine {

    // Immediate-mode line renderer for debug / collision visualization.
    // Call Draw* methods each frame to buffer lines, then Flush() to upload + draw.
    // All positions are in world space.
    class DEFINITELY_ENGINE_API DebugDraw {
    public:
        DebugDraw();
        ~DebugDraw();

        // Primitives
        void DrawLine(const glm::vec3& a, const glm::vec3& b, const glm::vec4& color);

        // Shapes
        void DrawBox(const glm::vec3& center, const glm::vec3& halfExtents, const glm::vec4& color);
        void DrawCapsule(const glm::vec3& bottomCenter, float radius, float height,
                         const glm::vec4& color, int segments = 16);

        // Upload buffered lines to GPU, draw, then clear for next frame.
        void Flush(const glm::mat4& viewProjection);

    private:
        struct LineVertex { glm::vec3 pos; glm::vec4 color; };

        std::vector<LineVertex> m_Buffer;

        Shader*       m_Shader       = nullptr;
        VertexArray*  m_VertexArray  = nullptr;
        VertexBuffer* m_VertexBuffer = nullptr;

        void DrawCircle(const glm::vec3& center, const glm::vec3& axisU,
                        const glm::vec3& axisV, float radius,
                        const glm::vec4& color, int segments);
        void DrawArc(const glm::vec3& center, const glm::vec3& axisU,
                     const glm::vec3& axisV, float radius,
                     float startAngle, float endAngle,
                     const glm::vec4& color, int segments);
    };

}
