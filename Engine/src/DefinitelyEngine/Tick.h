#pragma once

#include "depch.h"
#include "Core.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API Tick {
    public:
        Tick();

        void Update();

        float DeltaTime() const;

    private:
        std::chrono::steady_clock::time_point m_LastFrameTime;
        float m_DeltaTime = 0.0f;
    };

}
