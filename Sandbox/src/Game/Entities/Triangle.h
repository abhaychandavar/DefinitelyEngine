#pragma once
#include "DefinitelyEngine.h"

class Triangle {
public:
    Triangle();
    void Render();

private:
    void DrawTestTriangle();

    DefinitelyEngine::VertexBuffer* m_Buffer;
    DefinitelyEngine::Shader* m_shader;
};
