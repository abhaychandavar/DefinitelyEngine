#pragma once
#include "DefinitelyEngine/Renderer/RendererAPI.h"

namespace DefinitelyEngine {

    class OpenGLRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
        virtual void Clear() override;
        virtual void DrawArrays(PrimitiveType primitive, int count) override;
    };

}
