#include "GameLayer.h"

GameLayer::GameLayer()
    : m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f)
{
    // Suzanne
    m_SuzanneModel = new DefinitelyEngine::Model("Assets/Models/suzanne.fbx");
    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Suzanne";
        obj.transform.scale = 0.01f;
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_SuzanneModel->Draw(vp, t);
        };
        m_Objects.push_back(std::move(obj));
    }

    // Plane
    m_Plane = new Plane();
    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Plane";
        obj.transform.position = { 0.0f, -1.0f, 0.0f };
        obj.transform.scale = 5.0f;
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_Plane->Render(vp, t);
        };
        m_Objects.push_back(std::move(obj));
    }
}

GameLayer::~GameLayer() {
    delete m_SuzanneModel;
    delete m_Plane;
}

void GameLayer::OnUpdate() {
    m_Camera.OnUpdate(&m_Objects[0].transform.position);

    const glm::mat4& vp = m_Camera.GetCamera().GetViewProjectionMatrix();
    for (auto& obj : m_Objects)
        obj.Render(vp);
}

void GameLayer::OnAttach() {}
void GameLayer::OnDetach() {}
void GameLayer::OnEvent(DefinitelyEngine::Event& e) {}
