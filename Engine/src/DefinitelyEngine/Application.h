#pragma once

#include "Core.h"

namespace DefinitelyEngine
{
  class DEFINITELY_ENGINE_API Application
  {
    public:
      Application();
      virtual ~Application();

      void Run();
  };

  Application* CreateApplication();
} // namespace Engine