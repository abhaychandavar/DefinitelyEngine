#include "DefinitelyEngine.h"

class SandboxLayer : public DefinitelyEngine::Layer
{
public:
    SandboxLayer() : Layer("Sandbox") {}

    void OnUpdate() override
    {
    }

    void OnEvent(DefinitelyEngine::Event& e) override
    {
        if (e.GetType() == DefinitelyEngine::EventType::KeyPressed) {
            DefinitelyEngine::KeyPressedEvent& event = (DefinitelyEngine::KeyPressedEvent&)e;
            DE_TRACE("{0}", (char)event.GetKeyCode());
        }
    }
};

class Sandbox : public DefinitelyEngine::Application
{
    public:
        Sandbox()
        {
            PushLayer(new SandboxLayer());
        }
        ~Sandbox() {}
};

DefinitelyEngine::Application* DefinitelyEngine::CreateApplication() {
    return new Sandbox();
}
