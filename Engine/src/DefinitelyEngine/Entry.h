#pragma once

#include "DefinitelyEngine/Events/MouseEvent.cpp"

extern DefinitelyEngine::Application* DefinitelyEngine::CreateApplication();

int main(int argc, char** argv)
{
    DefinitelyEngine::Log::Init();
    DE_CLIENT_INFO("Definitely engine initialized");
    DE_CLIENT_INFO("Definitely engine initialized 2");

    DefinitelyEngine::MouseMove mouseMoveEvent({ 100.0f, 200.0f });
    DE_CLIENT_INFO("Fake mouse moved event: {}", mouseMoveEvent.ToString());

    auto app = DefinitelyEngine::CreateApplication();
    app->Run();
    delete app;
}