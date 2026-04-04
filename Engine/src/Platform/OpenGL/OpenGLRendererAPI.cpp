#include "depch.h"
#include "OpenGLRendererAPI.h"
#include "DefinitelyEngine/Log.h"

#include <glad/glad.h>

namespace DefinitelyEngine {

    void OpenGLRendererAPI::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void OpenGLRendererAPI::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::DrawArrays(PrimitiveType primitive, int count) {
        static auto ToGLPrimitive = [](PrimitiveType type) -> GLenum {
            switch (type) {
                case PrimitiveType::Triangles: return GL_TRIANGLES;
                case PrimitiveType::Lines:     return GL_LINE_STRIP;
                case PrimitiveType::Points:    return GL_POINTS;
            }
        };
        glDrawArrays(ToGLPrimitive(primitive), 0, count);
    }

}
