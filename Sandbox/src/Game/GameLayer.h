#pragma once

#include "DefinitelyEngine.h"
#include "Game/Entities/Triangle.h"

class GameLayer : public DefinitelyEngine::Layer {
    public:
        GameLayer();
        ~GameLayer();

        void OnUpdate() override;

        void OnEvent(DefinitelyEngine::Event& e) override;

        void OnAttach() override;

        void OnDetach() override;

    private:
        Triangle m_Triangle;
};