#pragma once

#include "DefinitelyEngine.h"

class EditorLayer : public DefinitelyEngine::Layer {
public:
    EditorLayer(std::vector<DefinitelyEngine::GameObject>& objects, int& selectedIndex);

    void OnAttach() override;
    void OnImGuiRender() override;

private:
    std::vector<DefinitelyEngine::GameObject>& m_Objects;
    int& m_SelectedIndex;
};
