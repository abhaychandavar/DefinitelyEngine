#pragma once
#include "DefinitelyEngine/Core.h"
#include "RendererAPI.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API RenderCommand {
    public:
        inline static void Init() {
            s_RendererAPI->Init();
        }

        inline static void SetClearColor(float r, float g, float b, float a) {
            s_RendererAPI->SetClearColor(r, g, b, a);
        }

        inline static void Clear() {
            s_RendererAPI->Clear();
        }

        inline static void DrawArrays(RendererAPI::PrimitiveType primitive, int count) {
            s_RendererAPI->DrawArrays(primitive, count);
        }

        inline static void DrawIndexed(RendererAPI::PrimitiveType primitive, unsigned int count) {
            s_RendererAPI->DrawIndexed(primitive, count);
        }

        inline static void SetDepthTestEnabled(bool enabled) {
            s_RendererAPI->SetDepthTestEnabled(enabled);
        }

        inline static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
            s_RendererAPI->SetViewport(x, y, width, height);
        }

        inline static void SetPointSize(float size) {
            s_RendererAPI->SetPointSize(size);
        }

    private:
        static RendererAPI* s_RendererAPI;
    };

}
