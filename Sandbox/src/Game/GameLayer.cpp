#include "GameLayer.h"

GameLayer::GameLayer() {}

GameLayer::~GameLayer() {}

void GameLayer::OnUpdate() {
    m_Triangle.Render();
}


void GameLayer::OnAttach() {}

void GameLayer::OnEvent(DefinitelyEngine::Event& e) {}

void GameLayer::OnDetach() {}