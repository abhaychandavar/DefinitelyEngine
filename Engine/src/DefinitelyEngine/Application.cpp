#include "depch.h"
#include "Application.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/WindowResizeEvent.h"
#include "Log.h"
#include "ImGui/ImGuiLayer.h"

namespace DefinitelyEngine {

    Application* Application::s_Instance = nullptr;

    class ApplicationLayer : public Layer {
    public:
        ApplicationLayer() : Layer("ApplicationLayer") {}

        void OnEvent(Event& e) override {
            switch (e.GetType()) {
                case EventType::MouseMoved: {
                    auto& me = static_cast<MouseMovedEvent&>(e);
                    DE_CORE_TRACE("MouseMoved: ({}, {})", me.GetX(), me.GetY());
                    break;
                }
                case EventType::MouseScrolled: {
                    auto& se = static_cast<MouseScrolledEvent&>(e);
                    DE_CORE_TRACE("MouseScrolled: ({}, {})", se.GetXOffset(), se.GetYOffset());
                    break;
                }
                case EventType::MouseButtonPressed: {
                    auto& mbe = static_cast<MouseButtonPressedEvent&>(e);
                    DE_CORE_TRACE("MouseButtonPressed: button {}", mbe.GetButton());
                    break;
                }
                case EventType::MouseButtonReleased: {
                    auto& mbe = static_cast<MouseButtonReleasedEvent&>(e);
                    DE_CORE_TRACE("MouseButtonReleased: button {}", mbe.GetButton());
                    break;
                }
                case EventType::KeyPressed: {
                    auto& ke = static_cast<KeyPressedEvent&>(e);
                    DE_CORE_TRACE("KeyPressed: keycode {} (repeats: {})", ke.GetKeyCode(), ke.GetRepeatCount());
                    break;
                }
                case EventType::KeyReleased: {
                    auto& ke = static_cast<KeyReleasedEvent&>(e);
                    DE_CORE_TRACE("KeyReleased: keycode {}", ke.GetKeyCode());
                    break;
                }
                default: break;
            }
        }
    };

    Application::Application()
    {
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback([this](Event& e){
            OnEvent(e); });
        PushLayer(new ApplicationLayer());
        PushOverlay(new ImGuiLayer());
    }

    Application::~Application()
    {

    }

    void Application::PushLayer(Layer* layer) {
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay) {
        m_LayerStack.PushOverlay(overlay);
    }

    void Application::OnEvent(Event& e) {
        // Window events are handled directly at the application level
        if (e.IsInCategory(EventCategory::Window)) {
            if (e.GetType() == EventType::WindowClose) {
                OnClose();
            } else if (e.GetType() == EventType::WindowResize) {
                auto& re = static_cast<WindowResizeEvent&>(e);
                DE_CORE_INFO("WindowResize: {}x{}", re.GetWidth(), re.GetHeight());
            }
            return;
        }

        // All other events traverse the layer stack back-to-front (overlays first)
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            (*it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::OnClose() {
        this->Running = false;
    }

    void Application::Run() {
        this->Running = true;
        while(this->Running) {
            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();
            m_Window->OnUpdate();
        }
    }
}
