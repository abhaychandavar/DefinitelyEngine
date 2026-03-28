#pragma once

extern DefinitelyEngine::Application* DefinitelyEngine::CreateApplication();

int main(int argc, char** argv)
{
    printf("Definitely Engine initialized\n");
    auto app = DefinitelyEngine::CreateApplication();
    app->Run();
    delete app;
}