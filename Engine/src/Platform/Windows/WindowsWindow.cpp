#include "depch.h"
#ifdef DE_PLATFORM_WINDOWS
#include "WindowsWindow.h"
#include "DefinitelyEngine/Log.h"
#include "DefinitelyEngine/Events/WindowClose.h"
#include "DefinitelyEngine/Events/WindowResizeEvent.h"
#include "DefinitelyEngine/Events/MouseEvent.h"
#include "DefinitelyEngine/Events/KeyEvent.h"


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

        DE_CORE_ASSERT(m_Window, "Window initialization failed");

        m_Context = std::unique_ptr<GraphicsContext>(GraphicsContext::Create(m_Window));
        m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowClose event;
            data.EventCallback(event);
        });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = (unsigned int)width;
            data.Height = (unsigned int)height;
            WindowResizeEvent event((unsigned int)width, (unsigned int)height);
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                int repeatCount = (action == GLFW_REPEAT) ? 1 : 0;
                KeyPressedEvent event(key, repeatCount);
                data.EventCallback(event);
            } else if (action == GLFW_RELEASE) {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xpos, (float)ypos);
            data.EventCallback(event);
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xoffset, (float)yoffset);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            if (action == GLFW_PRESS) {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
            } else if (action == GLFW_RELEASE) {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
            }
        });
    }

    void WindowsWindow::Shutdown() {
        glfwDestroyWindow(m_Window);
    }

    static void KeyCallback(int error, const char* description)
    {
        DE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    void WindowsWindow::OnUpdate() {
        glfwPollEvents();
        m_Context->SwapBuffers();
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
