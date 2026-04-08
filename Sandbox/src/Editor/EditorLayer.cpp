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

        if (obj.collider) {
            ImGui::Separator();
            ImGui::Text("Collider");
            ImGui::DragFloat3("Col Offset", glm::value_ptr(obj.collider->localOffset), 0.01f);
            if (obj.collider->GetType() == DefinitelyEngine::ColliderType::Box) {
                auto* b = static_cast<DefinitelyEngine::BoxCollider*>(obj.collider);
                ImGui::DragFloat3("Half Extents", glm::value_ptr(b->halfExtents), 0.01f, 0.001f, 50.0f);
            } else if (obj.collider->GetType() == DefinitelyEngine::ColliderType::Capsule) {
                auto* c = static_cast<DefinitelyEngine::CapsuleCollider*>(obj.collider);
                ImGui::DragFloat("Radius", &c->radius, 0.01f, 0.01f, 10.0f);
                ImGui::DragFloat("Height", &c->height, 0.01f, 0.01f, 10.0f);
            }
        }
    }
    ImGui::End();
}
