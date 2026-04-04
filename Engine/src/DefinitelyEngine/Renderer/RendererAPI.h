#pragma once
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {
    class DEFINITELY_ENGINE_API RendererAPI {
    public:
        enum class API {
            None   = 0,
            OpenGL = 1
        };

        enum class PrimitiveType {
            Triangles,
            Lines,
            Points
        };

        virtual void Init() = 0;
        virtual void SetClearColor(float r, float g, float b, float a) = 0;
        virtual void Clear() = 0;
        virtual void DrawArrays(PrimitiveType primitive, int count) = 0;

        inline static API GetAPI() { return s_API; }
        static RendererAPI* Create();

    private:
        static API s_API;
    };
}
