#pragma once

#include "Camera.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void SetPosition(const glm::vec3& position);
        void SetRotation(float yaw, float pitch);
        void SetFOV(float fov);
        void SetAspectRatio(float aspectRatio);

        const glm::vec3& GetPosition() const { return m_Position; }
        float GetYaw() const { return m_Yaw; }
        float GetPitch() const { return m_Pitch; }
        float GetFOV() const { return m_FOV; }
        float GetAspectRatio() const { return m_AspectRatio; }

        void SetTarget(glm::vec3* targetPosition, float radius);

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();
        void RecalculateTargetViewMatrix();

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Yaw   = -90.0f;
        float m_Pitch =   0.0f;

        float m_FOV;
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;

        // target
        glm::vec3* m_TargetPos = nullptr;
        float m_Radius;
    };

}
