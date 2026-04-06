#include "depch.h"
#include "Camera.h"

namespace DefinitelyEngine {

    void Camera::RecalculateViewProjection() {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}
