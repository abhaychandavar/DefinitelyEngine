#include "EditorLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

EditorLayer::EditorLayer(std::vector<DefinitelyEngine::GameObject>& objects, int& selectedIndex)
    : Layer("EditorLayer"), m_Objects(objects), m_SelectedIndex(selectedIndex)
{
}

void EditorLayer::OnAttach()
{
    ImGui::SetCurrentContext(DefinitelyEngine::ImGuiLayer::GetContext());
}

void EditorLayer::OnImGuiRender() {
    ImGui::Begin("Hierarchy");
    for (int i = 0; i < (int)m_Objects.size(); i++) {
        if (ImGui::Selectable(m_Objects[i].name.c_str(), m_SelectedIndex == i))
            m_SelectedIndex = i;
    }
    ImGui::End();

    ImGui::Begin("Transform");
    if (m_SelectedIndex >= 0 && m_SelectedIndex < (int)m_Objects.size()) {
        auto& obj = m_Objects[m_SelectedIndex];
        ImGui::Text("%s", obj.name.c_str());
        ImGui::Separator();
        ImGui::DragFloat3("Position", glm::value_ptr(obj.transform.position), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(obj.transform.rotation), 1.0f);
        ImGui::DragFloat("Scale",    &obj.transform.scale, 0.01f, 0.001f, 100.0f);
    }
    ImGui::End();
}
