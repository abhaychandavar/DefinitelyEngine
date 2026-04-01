#include "depch.h"
#include "Application.h"
#include "Events/Event.h"

namespace DefinitelyEngine {
    Application::Application()
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback([this](Event& e){ 
            OnEvent(e); });
    }

    Application::~Application()
    {

    }

    void Application::OnEvent(Event& e) {
        if (e.GetType() == EventType::WindowClose) {
            this->OnClose();
        }
    }

    
    void Application::OnEvent(Event& e) {
        this->Running = false;
    }

    void Application::Run() {
        this->Running = true;
        while(this->Running) {
            m_Window->OnUpdate();
        }
    }
}