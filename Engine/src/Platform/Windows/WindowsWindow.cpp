#include "depch.h"
#ifdef DE_PLATFORM_WINDOWS
#include "WindowsWindow.h"
#include "DefinitelyEngine/Log.h"

namespace DefinitelyEngine
{
    static bool s_GLFWInitialized = false;

    static void ErrorCallback(int error, const char* description)
    {
        DE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window* Window::Create(const WindowProps& props) {
        return new WindowsWindow(props);
    }

    WindowsWindow::WindowsWindow(const WindowProps& props) {
        Init(props);
    }
    WindowsWindow::~WindowsWindow() {
        Shutdown();
    }

    void WindowsWindow::Init(const WindowProps& props) {
        m_Data.Title = props.Title;
        m_Data.Height = props.Height;
        m_Data.Width = props.Width;

        DE_CORE_INFO("Creating new window title: {0}, Dimension: {1} x {2}", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized) {
            glfwSetErrorCallback(ErrorCallback);

            if (!glfwInit()) {
                DE_CORE_ERROR("GLFW initialization failed");
            }

            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            DE_CORE_ERROR("Window initialization failed");
        }

        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);
    }

    void WindowsWindow::Shutdown() {
        glfwDestroyWindow(m_Window);
    }

    static void KeyCallback(int error, const char* description)
    {
        DE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    void WindowsWindow::SetEventCallback() {
        
    }

    void WindowsWindow::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void WindowsWindow::SetVSync(bool enabled) {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.Vsync = enabled;
    }

    bool WindowsWindow::IsVsync() const {
        return m_Data.Vsync;
    }
} // namespace DefinitelyEngine
#endif // DE_PLATFORM_WINDOWS
