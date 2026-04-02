#include "DefinitelyEngine/Layer.h"

namespace DefinitelyEngine {
    class ImGuiLayer : public Layer {
        public:
            ImGuiLayer();
            ~ImGuiLayer();

            void OnAttach();
            void OnDetach();
            void OnUpdate();
            void OnEvent(Event& event);
    };
}