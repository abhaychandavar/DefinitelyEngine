#include "depch.h"
#include "Tick.h"

namespace DefinitelyEngine {
    Tick::Tick()
        : m_LastFrameTime(std::chrono::steady_clock::now())
    {
    }

    void Tick::Update() {
        auto now = std::chrono::steady_clock::now();
        m_DeltaTime = std::chrono::duration<float>(now - m_LastFrameTime).count();
        m_LastFrameTime = now;
    }

    float Tick::DeltaTime() const {
        return m_DeltaTime;
    }
}
