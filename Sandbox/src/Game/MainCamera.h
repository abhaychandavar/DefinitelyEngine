#pragma once

#include "DefinitelyEngine.h"

class MainCamera {
public:
    MainCamera(float fov, float aspectRatio, float nearClip, float farClip);

    void OnUpdate(glm::vec3* target);

    DefinitelyEngine::PerspectiveCamera& GetCamera() { return m_Camera; }

private:
    DefinitelyEngine::PerspectiveCamera m_Camera;

    float m_MouseSensitivity = 0.1f;
    float m_LastMouseX       = 0.0f;
    float m_LastMouseY       = 0.0f;
    bool  m_FirstMouse       = true;
};
