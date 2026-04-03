#include "depch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace DefinitelyEngine {

    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();

}
