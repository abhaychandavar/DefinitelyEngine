#include "MainCamera.h"
#include "DefinitelyEngine/Input.h"

MainCamera::MainCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_Camera(fov, aspectRatio, nearClip, farClip)
{
    m_Camera.SetPosition({ 0.0f, 0.0f, 3.0f });
}

void MainCamera::OnUpdate(glm::vec3* target) {
    m_Camera.SetTarget(target, 5);

    float yaw   = m_Camera.GetYaw();
    float pitch = m_Camera.GetPitch();

    float mouseX = DefinitelyEngine::Input::GetMouseX();
    float mouseY = DefinitelyEngine::Input::GetMouseY();

    if (m_FirstMouse) {
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;
        m_FirstMouse = false;
    }

    float dx = (mouseX - m_LastMouseX) * m_MouseSensitivity;
    float dy = (m_LastMouseY - mouseY) * m_MouseSensitivity;
    m_LastMouseX = mouseX;
    m_LastMouseY = mouseY;

    m_Camera.SetRotation(yaw + dx, glm::clamp(pitch + dy, -89.0f, 89.0f));
}
