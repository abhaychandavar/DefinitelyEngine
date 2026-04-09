#pragma once
#include "DefinitelyEngine/Renderer/RendererAPI.h"

namespace DefinitelyEngine {

    class OpenGLRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
        virtual void Clear() override;
        virtual void DrawArrays(PrimitiveType primitive, int count) override;
        virtual void DrawIndexed(PrimitiveType primitive, unsigned int count) override;
        virtual void SetDepthTestEnabled(bool enabled) override;
        virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
        virtual void SetPointSize(float size) override;
    };

}
