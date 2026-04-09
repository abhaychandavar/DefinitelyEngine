#include "depch.h"
#include "ParticleSystem.h"
#include "DefinitelyEngine/Renderer/VertexBufferLayout.h"
#include "DefinitelyEngine/Renderer/RenderCommand.h"
#include "DefinitelyEngine/Renderer/RendererAPI.h"
#include "DefinitelyEngine/Log.h"

#include <cstddef>
#include <cstdlib>  // rand, RAND_MAX

namespace DefinitelyEngine {

    static float RandF() {
        return (float)rand() / (float)RAND_MAX;  // [0, 1]
    }

    static float RandF(float lo, float hi) {
        return lo + RandF() * (hi - lo);
    }

    // Returns a random unit direction within a cone of half-angle `spread` around `axis`
    static glm::vec3 RandomConeDir(const glm::vec3& axis, float spread) {
        // Build orthonormal basis (u, v, axis)
        glm::vec3 up = (glm::abs(axis.y) < 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        glm::vec3 u  = glm::normalize(glm::cross(up, axis));
        glm::vec3 v  = glm::cross(axis, u);

        float phi   = RandF(0.0f, glm::two_pi<float>());   // azimuth
        float theta = RandF(0.0f, spread);                   // polar angle
        float sinT  = glm::sin(theta);

        return glm::normalize(axis * glm::cos(theta) + u * (sinT * glm::cos(phi)) + v * (sinT * glm::sin(phi)));
    }

    ParticleSystem::ParticleSystem(int maxParticles) {
        m_Pool.resize(maxParticles);
        m_UploadBuffer.reserve(maxParticles);

        m_Shader = Shader::CreateFromFile("Shaders/Particle.shader");
        DE_CORE_INFO(
            "Particle vertex layout: sizeof={0}, positionOffset={1}, colorOffset={2}, sizeOffset={3}",
            (int)sizeof(ParticleVertex),
            (int)offsetof(ParticleVertex, position),
            (int)offsetof(ParticleVertex, color),
            (int)offsetof(ParticleVertex, size));

        VertexBufferLayout layout;
        layout.Push(ShaderDataType::Float3, "a_Position");
        layout.Push(ShaderDataType::Float4, "a_Color");
        layout.Push(ShaderDataType::Float,  "a_Size");

        m_VertexBuffer = VertexBuffer::Create();
        m_VertexBuffer->SetLayout(layout);

        m_VertexArray = VertexArray::Create();
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->Unbind();
    }

    ParticleSystem::~ParticleSystem() {
        delete m_Shader;
        delete m_VertexArray;
        // VertexBuffer is owned by VertexArray
    }

    int ParticleSystem::FindFreeSlot() {
        int n = (int)m_Pool.size();
        for (int i = 0; i < n; ++i) {
            int idx = (m_Cursor + i) % n;
            if (!m_Pool[idx].active) {
                m_Cursor = (idx + 1) % n;
                return idx;
            }
        }
        // Pool exhausted: overwrite at cursor (oldest-first eviction)
        int idx  = m_Cursor;
        m_Cursor = (m_Cursor + 1) % n;
        return idx;
    }

    void ParticleSystem::Emit(glm::vec3 worldPos, glm::vec3 direction, const EmitterConfig& config) {
        glm::vec3 dir = (glm::length(direction) > 0.001f)
                            ? glm::normalize(direction)
                            : glm::vec3(0.0f, 1.0f, 0.0f);

        for (int i = 0; i < config.count; ++i) {
            int idx = FindFreeSlot();
            Particle& p = m_Pool[idx];

            float speed    = RandF(config.speed - config.speedVariance,
                                   config.speed + config.speedVariance);
            float lifetime = RandF(config.lifetime - config.lifetimeVariance,
                                   config.lifetime + config.lifetimeVariance);
            lifetime = glm::max(lifetime, 0.05f);

            p.position    = worldPos;
            p.velocity    = RandomConeDir(dir, config.spread) * glm::max(speed, 0.0f);
            p.colorStart  = config.colorStart;
            p.colorEnd    = config.colorEnd;
            p.sizeStart   = config.sizeStart;
            p.sizeEnd     = config.sizeEnd;
            p.lifetime    = lifetime;
            p.gravityAccel = config.gravity;
            p.age         = 0.0f;
            p.active      = true;
        }
    }

    void ParticleSystem::Update(float dt) {
        for (Particle& p : m_Pool) {
            if (!p.active) continue;

            p.age += dt;
            if (p.age >= p.lifetime) {
                p.active = false;
                continue;
            }

            p.velocity.y  += p.gravityAccel * dt;
            p.position    += p.velocity * dt;
        }
    }

    void ParticleSystem::Render(const glm::mat4& viewProjection) {
        m_UploadBuffer.clear();

        for (const Particle& p : m_Pool) {
            if (!p.active) continue;

            float t     = p.age / p.lifetime;
            glm::vec4 color = glm::mix(p.colorStart, p.colorEnd, t);
            float size      = glm::mix(p.sizeStart,  p.sizeEnd,  t);

            m_UploadBuffer.push_back({ p.position, color, size });
        }

        if (m_UploadBuffer.empty()) return;

        if (!m_LoggedRenderState) {
            const ParticleVertex& p = m_UploadBuffer.front();
            DE_CORE_INFO(
                "First particle upload: count={0}, pos=({1:.2f}, {2:.2f}, {3:.2f}), color=({4:.2f}, {5:.2f}, {6:.2f}, {7:.2f}), size={8:.2f}",
                (int)m_UploadBuffer.size(),
                p.position.x, p.position.y, p.position.z,
                p.color.r, p.color.g, p.color.b, p.color.a,
                p.size);
            m_LoggedRenderState = true;
        }

        m_VertexBuffer->SetData(m_UploadBuffer.data(),
                                (uint32_t)(m_UploadBuffer.size() * sizeof(ParticleVertex)),
                                BufferUsage::Stream);

        m_Shader->Bind();
        m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);
        m_VertexArray->Bind();

        // Set a fallback point size for when GL_PROGRAM_POINT_SIZE is not yet enabled.
        // When GL_PROGRAM_POINT_SIZE IS enabled, gl_PointSize in the shader takes over.
        RenderCommand::SetPointSize(16.0f);
        RenderCommand::DrawArrays(RendererAPI::PrimitiveType::Points, (int)m_UploadBuffer.size());
        RenderCommand::SetPointSize(1.0f);

        m_VertexArray->Unbind();
        m_Shader->Unbind();
    }

}
