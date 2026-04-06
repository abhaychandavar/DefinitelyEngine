#pragma once
#include "DefinitelyEngine.h"

class SquareIndexBuffer {
public:
    SquareIndexBuffer();
    void Render();

private:
    void DrawTestSquare();

    DefinitelyEngine::VertexArray* m_VertexArray;
    DefinitelyEngine::Shader* m_shader;

    float r, g, b;
};
