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

    private:
        static RendererAPI* s_RendererAPI;
    };

}
