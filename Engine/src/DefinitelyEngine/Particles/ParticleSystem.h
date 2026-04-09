#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "DefinitelyEngine/Renderer/Shader.h"
#include "DefinitelyEngine/Renderer/VertexArray.h"
#include "DefinitelyEngine/Renderer/VertexBuffer.h"

namespace DefinitelyEngine {

    struct DEFINITELY_ENGINE_API EmitterConfig {
        int       count            = 20;
        float     speed            = 4.0f;
        float     speedVariance    = 1.5f;
        float     spread           = 1.0f;    // cone half-angle in radians
        float     lifetime         = 0.6f;
        float     lifetimeVariance = 0.2f;
        glm::vec4 colorStart       = { 1.0f, 0.0f, 0.0f, 1.0f };
        glm::vec4 colorEnd         = { 0.3f, 0.0f, 0.0f, 0.0f };
        float     sizeStart        = 8.0f;    // gl_PointSize in pixels
        float     sizeEnd          = 2.0f;
        float     gravity          = -4.0f;   // Y acceleration (units/s^2)
    };

    class DEFINITELY_ENGINE_API ParticleSystem {
    public:
        explicit ParticleSystem(int maxParticles = 2000);
        ~ParticleSystem();

        // Spawn a burst at worldPos, ejecting roughly along direction
        void Emit(glm::vec3 worldPos, glm::vec3 direction, const EmitterConfig& config);

        // Age particles, apply velocity + gravity
        void Update(float dt);

        // Upload active particles to GPU and draw (call after world render, before HUD)
        void Render(const glm::mat4& viewProjection);

    private:
        struct Particle {
            glm::vec3 position;
            glm::vec3 velocity;
            glm::vec4 colorStart;
            glm::vec4 colorEnd;
            float     sizeStart;
            float     sizeEnd;
            float     lifetime;
            float     age;
            float     gravityAccel;
            bool      active = false;
        };

        struct ParticleVertex {
            glm::vec3 position;
            glm::vec4 color;
            float     size;
        };

        std::vector<Particle>       m_Pool;
        std::vector<ParticleVertex> m_UploadBuffer;
        int                         m_Cursor = 0;

        Shader*       m_Shader       = nullptr;
        VertexArray*  m_VertexArray  = nullptr;
        VertexBuffer* m_VertexBuffer = nullptr;
        bool          m_LoggedRenderState = false;

        int FindFreeSlot();
    };

}
