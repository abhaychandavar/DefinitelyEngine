#include "MainCamera.h"
#include "DefinitelyEngine.h"
#include "DefinitelyEngine/Input.h"

MainCamera::MainCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_Camera(fov, aspectRatio, nearClip, farClip)
{
    m_Camera.SetPosition({ 0.0f, 1.6f, 3.0f });
    m_Camera.SetRotation(m_BaseYaw, m_BasePitch);
    DefinitelyEngine::Application::Get().GetWindow().SetCursorLocked(true);
}

void MainCamera::TriggerShotShake() {
    m_ShotShakePitch += 1.6f;
    m_ShotShakeYaw   += (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.8f;
    m_ShotShakePitch = glm::clamp(m_ShotShakePitch, -4.0f, 4.0f);
    m_ShotShakeYaw   = glm::clamp(m_ShotShakeYaw, -2.0f, 2.0f);
}

void MainCamera::OnUpdate(float dt) {
    const bool isEscPressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_ESCAPE);
    if (isEscPressed && !m_WasEscapePressed) {
        m_CursorLocked = !m_CursorLocked;
        DefinitelyEngine::Application::Get().GetWindow().SetCursorLocked(m_CursorLocked);
        m_FirstMouse = true;
    }
    m_WasEscapePressed = isEscPressed;

    float yaw   = m_BaseYaw;
    float pitch = m_BasePitch;

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
    }

    m_BaseYaw = yaw;
    m_BasePitch = pitch;

    const float shakeBlend = 1.0f - glm::exp(-20.0f * dt);
    m_ShotShakeYaw = glm::mix(m_ShotShakeYaw, 0.0f, shakeBlend);
    m_ShotShakePitch = glm::mix(m_ShotShakePitch, 0.0f, shakeBlend);
    m_Camera.SetRotation(
        m_BaseYaw + m_ShotShakeYaw,
        glm::clamp(m_BasePitch + m_ShotShakePitch, -89.0f, 89.0f));

    glm::vec3 currPos = m_Camera.GetPosition();

    glm::vec3 flatFront = glm::normalize(glm::vec3(
        glm::cos(glm::radians(m_BaseYaw)), 0.0f, glm::sin(glm::radians(m_BaseYaw))));
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
