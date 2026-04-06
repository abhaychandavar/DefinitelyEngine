#pragma once

#include "Camera.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API OrthographicCamera : public Camera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetPosition(const glm::vec3& position);
        void SetRotation(float rotation);
        void SetBounds(float left, float right, float bottom, float top);

        const glm::vec3& GetPosition() const { return m_Position; }
        float GetRotation() const { return m_Rotation; }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;

        float m_Left, m_Right, m_Bottom, m_Top;
    };

}
