#include "depch.h"
#include "DefinitelyEngine/Window.h"
#include <GLFW/glfw3.h>

namespace DefinitelyEngine {
    class WindowsWindow : public Window {
        public:
            WindowsWindow(const WindowProps& props);
            virtual ~WindowsWindow();

            void OnUpdate() override;

            inline unsigned int GetWidth() const override { return m_Data.Width; }
            inline unsigned int GetHeight() const override { return m_Data.Height; }

            void SetVSync(bool enabled) override;
            bool IsVsync() const;

            void SetEventCallback(std::function<void(Event&)> func) override { this->m_Data.EventCallback = func; };

        private:
            virtual void Init(const WindowProps& props);
            virtual void Shutdown();
            
            GLFWwindow* m_Window;

            struct WindowData {
                std::string Title;
                unsigned int Width, Height;
                bool Vsync;

                std::function<void(Event&)> EventCallback;
            };

            WindowData m_Data;
    };
}