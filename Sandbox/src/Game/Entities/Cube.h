#pragma once
#include "DefinitelyEngine.h"

class Cube {
public:
    Cube();
    void Render(const glm::mat4& viewProjection);

private:
    void DrawTestSquare(const glm::mat4& viewProjection);

    DefinitelyEngine::VertexArray* m_VertexArray;
    DefinitelyEngine::Shader* m_shader;

    float r, g, b;
};
