#pragma once
#include "DefinitelyEngine.h"

class SquareTextured {
public:
    SquareTextured();
    void Render();

private:
    void DrawTestSquare();

    DefinitelyEngine::VertexArray* m_VertexArray;
    DefinitelyEngine::Shader* m_shader;

    float r, g, b;
};
