#include "depch.h"
#include "OpenGLRendererAPI.h"
#include "OpenGLDebug.h"
#include "DefinitelyEngine/Log.h"

#include <glad/glad.h>

namespace {
    GLenum ToGLPrimitive(DefinitelyEngine::RendererAPI::PrimitiveType type) {
        switch (type) {
            case DefinitelyEngine::RendererAPI::PrimitiveType::Triangles: return GL_TRIANGLES;
            case DefinitelyEngine::RendererAPI::PrimitiveType::Lines:     return GL_LINE_STRIP;
            case DefinitelyEngine::RendererAPI::PrimitiveType::Points:    return GL_POINTS;
        }
        return GL_TRIANGLES;
    }
}

namespace DefinitelyEngine {

    void OpenGLRendererAPI::Init() {
        InitOpenGLDebug();

        glEnable(GL_DEPTH_TEST);

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
        glDrawArrays(ToGLPrimitive(primitive), 0, count);
    }

    void OpenGLRendererAPI::DrawIndexed(PrimitiveType primitive, unsigned int count) {
        glDrawElements(ToGLPrimitive(primitive), count, GL_UNSIGNED_INT, nullptr);
    }

}
