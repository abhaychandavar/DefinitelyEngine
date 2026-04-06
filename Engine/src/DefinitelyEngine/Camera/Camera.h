#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API Camera {
    public:
        virtual ~Camera() = default;

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    protected:
        void RecalculateViewProjection();

        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);
    };

}
