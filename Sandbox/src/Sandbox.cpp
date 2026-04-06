#include "DefinitelyEngine.h"

#include "DefinitelyEngine/Entry.h"
#include "Game/GameLayer.h"
#include "Editor/EditorLayer.h"

class Sandbox : public DefinitelyEngine::Application
{
    public:
        Sandbox()
        {
            auto* gameLayer = new GameLayer();
            PushLayer(gameLayer);
            PushLayer(new EditorLayer(gameLayer->GetObjects(), gameLayer->GetSelectedIndex()));
        }
        ~Sandbox() {}
};

DefinitelyEngine::Application* DefinitelyEngine::CreateApplication() {
    return new Sandbox();
}
