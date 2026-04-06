#pragma once

#include "DefinitelyEngine.h"
#include "MainCamera.h"
#include "Game/Entities/Plane.h"

class GameLayer : public DefinitelyEngine::Layer {
public:
    GameLayer();
    ~GameLayer();

    void OnUpdate() override;
    void OnEvent(DefinitelyEngine::Event& e) override;
    void OnAttach() override;
    void OnDetach() override;

    std::vector<DefinitelyEngine::GameObject>& GetObjects() { return m_Objects; }
    int& GetSelectedIndex() { return m_SelectedIndex; }

private:
    std::vector<DefinitelyEngine::GameObject> m_Objects;
    int m_SelectedIndex = -1;

    DefinitelyEngine::Model* m_SuzanneModel = nullptr;
    Plane* m_Plane = nullptr;

    MainCamera m_Camera;
};
