#pragma once

#include "Core.h"
#include "Window.h"

namespace DefinitelyEngine
{
  class DEFINITELY_ENGINE_API Application
  {
    public:
      Application();
      virtual ~Application();

      void Run();

    private:
      std::unique_ptr<Window> m_Window;
      bool Running;
      
      void OnEvent(Event& e);

      void OnClose();
  };

  Application* CreateApplication();
} // namespace Engine