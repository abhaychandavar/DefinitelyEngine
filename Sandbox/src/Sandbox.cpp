#include "DefinitelyEngine.h"

class Sandbox : public DefinitelyEngine::Application
{
    public:
        Sandbox() {}
        ~Sandbox() {}
};

DefinitelyEngine::Application* DefinitelyEngine::CreateApplication() {
    return new Sandbox();
}
