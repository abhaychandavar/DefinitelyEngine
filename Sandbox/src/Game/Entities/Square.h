#pragma once
#include "DefinitelyEngine.h"

class Square {
public:
    Square();
    void Render();

private:
    void DrawTestSquare();

    DefinitelyEngine::VertexBuffer* m_Buffer;
    DefinitelyEngine::VertexArray* m_VertexArray;
    DefinitelyEngine::Shader* m_shader;
};
