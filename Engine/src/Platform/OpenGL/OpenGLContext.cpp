#include "depch.h"
#include "OpenGLContext.h"
#include "DefinitelyEngine/Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace DefinitelyEngine {

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        DE_CORE_ASSERT(windowHandle, "Window handle is null");
    }

    void OpenGLContext::Init() {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        DE_CORE_ASSERT(status, "Failed to initialize GLAD");

        DE_CORE_INFO("OpenGL Renderer: {0}", (const char*)glGetString(GL_RENDERER));
        DE_CORE_INFO("OpenGL Version:  {0}", (const char*)glGetString(GL_VERSION));
    }

    void OpenGLContext::SwapBuffers() {
        glfwSwapBuffers(m_WindowHandle);
    }

    GraphicsContext* GraphicsContext::Create(void* window) {
        return new OpenGLContext(static_cast<GLFWwindow*>(window));
    }

}
