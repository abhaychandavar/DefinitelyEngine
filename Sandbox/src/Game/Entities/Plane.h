#pragma once
#include "DefinitelyEngine.h"

class Plane {
public:
    Plane();
    void Render(const glm::mat4& viewProjection, const glm::mat4& transform);

private:
    void DrawRect(const glm::mat4& viewProjection, const glm::mat4& transform);

    DefinitelyEngine::VertexArray* m_VertexArray;
    DefinitelyEngine::Shader* m_shader;

    float r, g, b;
};
