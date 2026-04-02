#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

namespace DefinitelyEngine
{
  class DEFINITELY_ENGINE_API Application
  {
    public:
      Application();
      virtual ~Application();

      void Run();

      void PushLayer(Layer* layer);
      void PushOverlay(Layer* overlay);

      inline Window& GetWindow() { return *m_Window; }
      inline static Application& Get() { return *s_Instance; }

    private:
      std::unique_ptr<Window> m_Window;
      LayerStack m_LayerStack;
      bool Running;

      void OnEvent(Event& e);
      void OnClose();

      static Application* s_Instance;
  };

  Application* CreateApplication();
} // namespace Engine