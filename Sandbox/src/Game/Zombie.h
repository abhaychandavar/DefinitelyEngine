#pragma once

#include "DefinitelyEngine.h"
#include "DefinitelyEngine/Particles/ParticleSystem.h"

class Zombie {
public:
    Zombie(DefinitelyEngine::AnimatedModel* model,
           const DefinitelyEngine::AnimationClip* runClip,
           const DefinitelyEngine::AnimationClip* attackClip,
           float attackDuration,
           float attackHitLogStartTime,
           const glm::vec3& position,
           const std::string& nameSuffix);
    ~Zombie();

    void AddToScene(std::vector<DefinitelyEngine::GameObject>& objects);
    void RegisterColliders(DefinitelyEngine::CollisionWorld& world,
                           std::vector<DefinitelyEngine::GameObject>& objects);

    void UpdateAnimation(float dt);
    void UpdateAI(float dt,
                  const glm::vec3& playerPos,
                  std::vector<DefinitelyEngine::GameObject>& objects,
                  float moveSpeed,
                  float attackRange,
                  float attackCooldownDuration);
    void UpdateGravity(float dt,
                       DefinitelyEngine::CollisionWorld& world,
                       std::vector<DefinitelyEngine::GameObject>& objects,
                       float gravity);
    void SyncColliders(std::vector<DefinitelyEngine::GameObject>& objects,
                       const std::string& handBoneName);

    void ApplyShotDamage(float damage,
                         DefinitelyEngine::CollisionWorld& world,
                         std::vector<DefinitelyEngine::GameObject>& objects,
                         DefinitelyEngine::ParticleSystem* particles,
                         const DefinitelyEngine::EmitterConfig& bloodConfig,
                         const DefinitelyEngine::EmitterConfig& deathConfig,
                         const DefinitelyEngine::RaycastHit& hit,
                         const glm::vec3& shotDirection);

    float TryApplyPlayerHit(const DefinitelyEngine::Contact& contact,
                            DefinitelyEngine::CapsuleCollider* playerCollider,
                            const DefinitelyEngine::PerspectiveCamera& playerCamera,
                            DefinitelyEngine::ParticleSystem* particles,
                            const DefinitelyEngine::EmitterConfig& playerHitConfig,
                            float damage);

    bool IsAlive() const { return m_Alive; }
    bool OwnsCollider(const DefinitelyEngine::Collider* collider) const;
    bool IsBodyCollider(const DefinitelyEngine::Collider* collider) const { return collider == m_BodyCollider; }
    DefinitelyEngine::CapsuleCollider* GetHandCollider() const { return m_HandCollider; }

private:
    enum class State { Follow, Attacking };

    void Kill(DefinitelyEngine::CollisionWorld& world,
              std::vector<DefinitelyEngine::GameObject>& objects,
              DefinitelyEngine::ParticleSystem* particles,
              const DefinitelyEngine::EmitterConfig& deathConfig);

    DefinitelyEngine::AnimatedModel* m_Model = nullptr;
    DefinitelyEngine::Animator* m_Animator = nullptr;
    const DefinitelyEngine::AnimationClip* m_RunClip = nullptr;
    const DefinitelyEngine::AnimationClip* m_AttackClip = nullptr;
    float m_AttackDuration = 0.0f;
    float m_AttackHitLogStartTime = 0.0f;
    glm::vec3 m_SpawnPosition = { 0.0f, 0.0f, 0.0f };

    int m_ObjectIndex = -1;
    std::string m_NameSuffix;

    DefinitelyEngine::CapsuleCollider* m_BodyCollider = nullptr;
    DefinitelyEngine::BoxCollider* m_HitCollider = nullptr;
    DefinitelyEngine::CapsuleCollider* m_HandCollider = nullptr;

    State m_State = State::Follow;
    float m_AttackTimeLeft = 0.0f;
    float m_AttackCooldown = 0.0f;
    float m_VertVelocity = 0.0f;
    float m_Health = 100.0f;
    bool m_Grounded = false;
    bool m_HandHitLoggedThisAttack = false;
    bool m_Alive = true;
};
