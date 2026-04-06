#include "depch.h"
#include "PerspectiveCamera.h"

namespace DefinitelyEngine {

    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
    {
        RecalculateProjectionMatrix();
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::SetRotation(float yaw, float pitch) {
        m_Yaw   = yaw;
        m_Pitch = pitch;
        RecalculateViewMatrix();
    }

    void PerspectiveCamera::SetFOV(float fov) {
        m_FOV = fov;
        RecalculateProjectionMatrix();
    }

    void PerspectiveCamera::SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
        RecalculateProjectionMatrix();
    }

    void PerspectiveCamera::RecalculateProjectionMatrix() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
        RecalculateViewProjection();
    }

    void PerspectiveCamera::SetTarget(glm::vec3* targetPos, float radius) {
        m_TargetPos = targetPos;
        m_Radius = radius;
    }

    void PerspectiveCamera::RecalculateViewMatrix() {
        if (m_TargetPos == nullptr) {
            glm::vec3 front;
            front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            front.y = sin(glm::radians(m_Pitch));
            front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
            front = glm::normalize(front);
            m_ViewMatrix = glm::lookAt(m_Position, m_Position + front, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else {
            RecalculateTargetViewMatrix();
        }
        RecalculateViewProjection();
    }

    void PerspectiveCamera::RecalculateTargetViewMatrix() {
        glm::vec3 offset;
        offset.x = m_Radius * cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        offset.y = m_Radius * sin(glm::radians(m_Pitch));
        offset.z = m_Radius * sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        glm::vec3 desiredPos = *m_TargetPos - offset;
        
        m_Position = desiredPos;
        m_ViewMatrix = glm::lookAt(m_Position, *m_TargetPos, glm::vec3(0.0f, 1.0f, 0.0f));
    }

}
