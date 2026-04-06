#pragma once
#include "DefinitelyEngine/Layer.h"

struct ImGuiContext;

namespace DefinitelyEngine {
    class ImGuiLayer : public Layer {
        public:
            ImGuiLayer();
            ~ImGuiLayer();

            virtual void OnAttach() override;
            virtual void OnDetach() override;
            virtual void OnImGuiRender() override;

            void Begin();
            void End();

            static DEFINITELY_ENGINE_API ImGuiContext* GetContext();
    };
}