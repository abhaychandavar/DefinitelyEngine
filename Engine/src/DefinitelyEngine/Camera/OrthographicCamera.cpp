#include "depch.h"
#include "OrthographicCamera.h"

namespace DefinitelyEngine {

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top)
    {
        RecalculateProjectionMatrix();
        RecalculateViewMatrix();
    }

    void OrthographicCamera::SetPosition(const glm::vec3& position) {
        m_Position = position;
        RecalculateViewMatrix();
    }

    void OrthographicCamera::SetRotation(float rotation) {
        m_Rotation = rotation;
        RecalculateViewMatrix();
    }

    void OrthographicCamera::SetBounds(float left, float right, float bottom, float top) {
        m_Left = left; m_Right = right;
        m_Bottom = bottom; m_Top = top;
        RecalculateProjectionMatrix();
    }

    void OrthographicCamera::RecalculateViewMatrix() {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

        m_ViewMatrix = glm::inverse(transform);
        RecalculateViewProjection();
    }

    void OrthographicCamera::RecalculateProjectionMatrix() {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, -1.0f, 1.0f);
        RecalculateViewProjection();
    }

}
