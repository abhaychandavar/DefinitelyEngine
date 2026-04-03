#pragma once

#include "DefinitelyEngine/Events/MouseEvent.cpp"

extern DefinitelyEngine::Application* DefinitelyEngine::CreateApplication();

int main(int argc, char** argv)
{
    DefinitelyEngine::Log::Init();
    DE_INFO("Definitely engine initialized");
    
    auto app = DefinitelyEngine::CreateApplication();
    app->Run();
    delete app;
}