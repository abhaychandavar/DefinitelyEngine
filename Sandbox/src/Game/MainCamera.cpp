#include "MainCamera.h"
#include "DefinitelyEngine.h"
#include "DefinitelyEngine/Input.h"

MainCamera::MainCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_Camera(fov, aspectRatio, nearClip, farClip)
{
    m_Camera.SetPosition({ 0.0f, 1.6f, 3.0f });
    DefinitelyEngine::Application::Get().GetWindow().SetCursorLocked(true);
}

void MainCamera::OnUpdate(float dt) {
    // Escape toggles cursor lock
    const bool isEscPressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_ESCAPE);
    if (isEscPressed && !m_WasEscapePressed) {
        m_CursorLocked = !m_CursorLocked;
        DefinitelyEngine::Application::Get().GetWindow().SetCursorLocked(m_CursorLocked);
        m_FirstMouse = true;  // avoid jump on re-lock
    }
    m_WasEscapePressed = isEscPressed;

    float yaw   = m_Camera.GetYaw();
    float pitch = m_Camera.GetPitch();

    if (m_CursorLocked) {
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

        yaw += dx;
        pitch = glm::clamp(pitch + dy, -89.0f, 89.0f);
        m_Camera.SetRotation(yaw, pitch);
    }

    glm::vec3 currPos = m_Camera.GetPosition();

    // Flat horizontal front vector (yaw only) — W/S must not change Y so gravity can work.
    glm::vec3 flatFront = glm::normalize(glm::vec3(
        glm::cos(glm::radians(yaw)), 0.0f, glm::sin(glm::radians(yaw))));
    glm::vec3 right = glm::normalize(glm::cross(flatFront, { 0.0f, 1.0f, 0.0f }));

    bool isWPressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_W);
    bool isSPressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_S);
    bool isAPressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_A);
    bool isDPressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_D);

    if (isWPressed) {
        m_Camera.SetPosition(currPos + flatFront * 5.0f * dt);
    }
    if (isSPressed) {
        m_Camera.SetPosition(currPos - flatFront * 5.0f * dt);
    }
    
    if (isDPressed) {
        m_Camera.SetPosition(currPos + right * 5.0f * dt);
    }
    if (isAPressed) {
        m_Camera.SetPosition(currPos - right * 5.0f * dt);
    }
}
