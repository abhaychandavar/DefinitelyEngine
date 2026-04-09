#include "Zombie.h"
#include <cstdlib>

namespace {
    float Rand01() {
        return (float)rand() / (float)RAND_MAX;
    }

    glm::vec3 ExtractTranslation(const glm::mat4& transform) {
        return glm::vec3(transform[3]);
    }
}

Zombie::Zombie(DefinitelyEngine::AnimatedModel* model,
               const DefinitelyEngine::AnimationClip* runClip,
               const DefinitelyEngine::AnimationClip* attackClip,
               float attackDuration,
               float attackHitLogStartTime,
               const glm::vec3& position,
               const std::string& nameSuffix)
    : m_Model(model),
      m_RunClip(runClip),
      m_AttackClip(attackClip),
      m_AttackDuration(attackDuration),
      m_AttackHitLogStartTime(attackHitLogStartTime),
      m_SpawnPosition(position),
      m_NameSuffix(nameSuffix)
{
    m_Animator = new DefinitelyEngine::Animator(&m_Model->GetSkeleton());
    if (m_RunClip)
        m_Animator->SetClip(m_RunClip);

    m_SpeedMultiplier = 1.0f + Rand01() * 0.5f;
    m_ApproachSideBias = -1.5f + Rand01() * 3.0f;
    m_PreferredAttackDistance = 0.9f + Rand01() * 0.8f;
}

Zombie::~Zombie() {
    delete m_Animator;
    delete m_BodyCollider;
    delete m_HitCollider;
    delete m_HandCollider;
}

void Zombie::AddToScene(std::vector<DefinitelyEngine::GameObject>& objects) {
    DefinitelyEngine::GameObject obj;
    obj.name = "Zombie_" + m_NameSuffix;
    obj.tag = "Enemy";
    obj.transform.position = m_SpawnPosition;
    obj.transform.scale = 0.01f;
    obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
        if (!m_Alive || !m_Animator)
            return;
        m_Model->Draw(vp, t, m_Animator->GetBoneMatrices());
    };
    objects.push_back(std::move(obj));
    m_ObjectIndex = (int)objects.size() - 1;
}

void Zombie::RegisterColliders(DefinitelyEngine::CollisionWorld& world,
                               std::vector<DefinitelyEngine::GameObject>& objects) {
    m_BodyCollider = new DefinitelyEngine::CapsuleCollider();
    m_BodyCollider->ownerName   = "Zombie_" + m_NameSuffix;
    m_BodyCollider->tag         = "Enemy";
    m_BodyCollider->localOffset = { 0.010f, 0.270f, 0.000f };
    m_BodyCollider->radius      = 0.3f;
    m_BodyCollider->height      = 1.2f;
    objects[m_ObjectIndex].collider = m_BodyCollider;
    world.Register(m_BodyCollider);

    m_HitCollider = new DefinitelyEngine::BoxCollider();
    m_HitCollider->ownerName   = "ZombieHitbox_" + m_NameSuffix;
    m_HitCollider->tag         = "Enemy";
    m_HitCollider->localOffset = { 0.0f, 0.9f, 0.0f };
    m_HitCollider->halfExtents = { 0.45f, 0.9f, 0.45f };
    world.Register(m_HitCollider);

    m_HandCollider = new DefinitelyEngine::CapsuleCollider();
    m_HandCollider->ownerName = "ZombieHandTrigger_" + m_NameSuffix;
    m_HandCollider->tag       = "ZombieHandTrigger";
    m_HandCollider->isTrigger = true;
    m_HandCollider->radius    = 0.18f;
    m_HandCollider->height    = 0.02f;
    world.Register(m_HandCollider);
}

void Zombie::UpdateAnimation(float dt) {
    if (m_Alive && m_Animator)
        m_Animator->Update(dt);
}

void Zombie::UpdateAI(float dt,
                      const glm::vec3& playerPos,
                      std::vector<DefinitelyEngine::GameObject>& objects,
                      float moveSpeed,
                      float attackRange,
                      float attackCooldownDuration) {
    if (!m_Alive)
        return;

    glm::vec3& zombiePos = objects[m_ObjectIndex].transform.position;
    glm::vec2 toPlayer = { playerPos.x - zombiePos.x, playerPos.z - zombiePos.z };
    float xzDist = glm::length(toPlayer);

    if (xzDist > 0.01f)
        objects[m_ObjectIndex].transform.rotation.y = glm::degrees(glm::atan(toPlayer.x, toPlayer.y));

    glm::vec2 forward2D = xzDist > 0.001f ? (toPlayer / xzDist) : glm::vec2(0.0f, 1.0f);
    glm::vec2 right2D = { -forward2D.y, forward2D.x };
    glm::vec2 desiredTargetXZ = {
        playerPos.x + right2D.x * m_ApproachSideBias,
        playerPos.z + right2D.y * m_ApproachSideBias
    };
    glm::vec2 desiredDelta = desiredTargetXZ - glm::vec2(zombiePos.x, zombiePos.z);
    float desiredDist = glm::length(desiredDelta);
    glm::vec2 desiredDir = desiredDist > 0.001f ? (desiredDelta / desiredDist) : forward2D;

    const bool inAttackRange = xzDist < attackRange;
    const float followStopDistance = glm::max(m_PreferredAttackDistance, m_BodyCollider->radius + 0.45f);
    const float actualSpeed = moveSpeed * m_SpeedMultiplier;
    const float attackLungeSpeed = actualSpeed * 0.65f;

    if (m_State == State::Attacking) {
        if (m_AttackTimeLeft > 0.0f) {
            m_AttackLungeDirection = xzDist > 0.001f ? forward2D : m_AttackLungeDirection;
            zombiePos.x += m_AttackLungeDirection.x * attackLungeSpeed * dt;
            zombiePos.z += m_AttackLungeDirection.y * attackLungeSpeed * dt;
            m_AttackTimeLeft -= dt;
            if (m_AttackTimeLeft <= 0.0f) {
                m_AttackTimeLeft = 0.0f;
                m_AttackCooldown = attackCooldownDuration;
                m_HandHitLoggedThisAttack = false;
            }
        } else {
            if (m_AttackCooldown > 0.0f)
                m_AttackCooldown -= dt;
            if (m_AttackCooldown <= 0.0f && m_AttackClip) {
                m_AttackCooldown = 0.0f;
                m_AttackTimeLeft = m_AttackDuration;
                m_HandHitLoggedThisAttack = false;
                m_Animator->SetClip(m_AttackClip, false);
            }
        }
    } else {
        if (m_AttackCooldown > 0.0f)
            m_AttackCooldown -= dt;
        if (inAttackRange && m_AttackCooldown <= 0.0f && m_AttackClip) {
            m_State = State::Attacking;
            m_AttackTimeLeft = m_AttackDuration;
            m_AttackCooldown = 0.0f;
            m_HandHitLoggedThisAttack = false;
            m_AttackLungeDirection = xzDist > 0.001f ? forward2D : m_AttackLungeDirection;
            m_Animator->SetClip(m_AttackClip, false);
        } else if (desiredDist > followStopDistance) {
            zombiePos.x += desiredDir.x * actualSpeed * dt;
            zombiePos.z += desiredDir.y * actualSpeed * dt;
        }
    }
}

void Zombie::UpdateGravity(float dt,
                           DefinitelyEngine::CollisionWorld& world,
                           std::vector<DefinitelyEngine::GameObject>& objects,
                           float gravity) {
    if (!m_Alive)
        return;

    glm::vec3& zombiePos = objects[m_ObjectIndex].transform.position;
    DefinitelyEngine::Ray downRay = { { zombiePos.x, zombiePos.y + 0.1f, zombiePos.z }, { 0.0f, -1.0f, 0.0f } };
    DefinitelyEngine::RaycastHit hit;
    bool onGround = world.Raycast(downRay, 0.5f, hit, "Enemy")
                    && hit.tag == "Ground"
                    && m_VertVelocity <= 0.0f;

    if (onGround) {
        m_Grounded = true;
        m_VertVelocity = 0.0f;
        zombiePos.y = hit.point.y;
    } else {
        m_Grounded = false;
        m_VertVelocity = glm::max(m_VertVelocity + gravity * dt, -15.0f);
        zombiePos.y += m_VertVelocity * dt;
    }
}

void Zombie::SyncColliders(std::vector<DefinitelyEngine::GameObject>& objects,
                           const std::string& handBoneName) {
    if (!m_Alive)
        return;

    const glm::vec3 basePos = objects[m_ObjectIndex].transform.position;
    m_BodyCollider->worldCenter = basePos + m_BodyCollider->localOffset;
    m_HitCollider->worldCenter = basePos + m_HitCollider->localOffset;

    glm::mat4 handBoneTransform(1.0f);
    if (m_Animator->TryGetNodeGlobalTransform(handBoneName, handBoneTransform)) {
        const glm::mat4 zombieWorld = objects[m_ObjectIndex].transform.GetMatrix();
        m_HandCollider->worldCenter = ExtractTranslation(zombieWorld * handBoneTransform);
    } else {
        m_HandCollider->worldCenter = basePos;
    }
}

void Zombie::ApplyShotDamage(float damage,
                             DefinitelyEngine::CollisionWorld& world,
                             std::vector<DefinitelyEngine::GameObject>& objects,
                             DefinitelyEngine::ParticleSystem* particles,
                             const DefinitelyEngine::EmitterConfig& bloodConfig,
                             const DefinitelyEngine::EmitterConfig& deathConfig,
                             const DefinitelyEngine::RaycastHit& hit,
                             const glm::vec3& shotDirection) {
    if (!m_Alive)
        return;

    m_Health = glm::max(0.0f, m_Health - damage);
    if (particles)
        particles->Emit(hit.point, shotDirection, bloodConfig);

    if (m_Health <= 0.0f)
        Kill(world, objects, particles, deathConfig);
}

float Zombie::TryApplyPlayerHit(const DefinitelyEngine::Contact& contact,
                                DefinitelyEngine::CapsuleCollider* playerCollider,
                                const DefinitelyEngine::PerspectiveCamera& playerCamera,
                                DefinitelyEngine::ParticleSystem* particles,
                                const DefinitelyEngine::EmitterConfig& playerHitConfig,
                                float damage) {
    if (!m_Alive)
        return 0.0f;

    const bool handContact =
        (contact.a == m_HandCollider && contact.b == playerCollider) ||
        (contact.b == m_HandCollider && contact.a == playerCollider);
    if (!handContact || m_State != State::Attacking)
        return 0.0f;

    const float elapsed = m_AttackDuration - m_AttackTimeLeft;
    if (elapsed < m_AttackHitLogStartTime || m_HandHitLoggedThisAttack)
        return 0.0f;

    m_HandHitLoggedThisAttack = true;
    if (particles)
        particles->Emit(playerCamera.GetPosition(), glm::vec3(0, 1, 0), playerHitConfig);
    return damage;
}

bool Zombie::OwnsCollider(const DefinitelyEngine::Collider* collider) const {
    return collider == m_BodyCollider || collider == m_HitCollider || collider == m_HandCollider;
}

void Zombie::Kill(DefinitelyEngine::CollisionWorld& world,
                  std::vector<DefinitelyEngine::GameObject>& objects,
                  DefinitelyEngine::ParticleSystem* particles,
                  const DefinitelyEngine::EmitterConfig& deathConfig) {
    m_Alive = false;
    m_State = State::Follow;
    m_AttackTimeLeft = 0.0f;
    m_AttackCooldown = 0.0f;
    m_HandHitLoggedThisAttack = false;

    world.Unregister(m_BodyCollider->ownerName);
    world.Unregister(m_HitCollider->ownerName);
    world.Unregister(m_HandCollider->ownerName);

    m_BodyCollider->tag.clear();
    m_HitCollider->tag.clear();
    m_HandCollider->tag.clear();
    objects[m_ObjectIndex].onRender = {};

    if (particles) {
        const glm::vec3 deathPos = objects[m_ObjectIndex].transform.position + glm::vec3(0, 0.8f, 0);
        particles->Emit(deathPos, glm::vec3(0, 1, 0), deathConfig);
    }
}
