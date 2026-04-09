#include "GameLayer.h"
#include "DefinitelyEngine/Input.h"
#include "DefinitelyEngine/Events/WindowResizeEvent.h"

#include <string>

static DefinitelyEngine::EmitterConfig MakeBloodHitConfig() {
    DefinitelyEngine::EmitterConfig cfg;
    cfg.count = 25; cfg.speed = 5.0f; cfg.speedVariance = 2.0f;
    cfg.spread = 0.8f; cfg.lifetime = 0.5f; cfg.lifetimeVariance = 0.2f;
    cfg.colorStart = { 0.8f, 0.0f, 0.0f, 1.0f };
    cfg.colorEnd   = { 0.3f, 0.0f, 0.0f, 0.0f };
    cfg.sizeStart = 10.0f; cfg.sizeEnd = 2.0f; cfg.gravity = -6.0f;
    return cfg;
}

static DefinitelyEngine::EmitterConfig MakePlayerHitConfig() {
    DefinitelyEngine::EmitterConfig cfg;
    cfg.count = 12; cfg.speed = 3.0f; cfg.speedVariance = 1.0f;
    cfg.spread = 1.2f; cfg.lifetime = 0.3f; cfg.lifetimeVariance = 0.1f;
    cfg.colorStart = { 1.0f, 0.6f, 0.1f, 1.0f };
    cfg.colorEnd   = { 1.0f, 0.2f, 0.0f, 0.0f };
    cfg.sizeStart = 8.0f; cfg.sizeEnd = 1.0f; cfg.gravity = -3.0f;
    return cfg;
}

static DefinitelyEngine::EmitterConfig MakeZombieDeathConfig() {
    DefinitelyEngine::EmitterConfig cfg;
    cfg.count = 60; cfg.speed = 6.0f; cfg.speedVariance = 3.0f;
    cfg.spread = glm::pi<float>();
    cfg.lifetime = 1.0f; cfg.lifetimeVariance = 0.4f;
    cfg.colorStart = { 0.9f, 0.0f, 0.0f, 1.0f };
    cfg.colorEnd   = { 0.2f, 0.0f, 0.0f, 0.0f };
    cfg.sizeStart = 14.0f; cfg.sizeEnd = 3.0f; cfg.gravity = -5.0f;
    return cfg;
}

static DefinitelyEngine::EmitterConfig MakeGunMuzzleConfig() {
    DefinitelyEngine::EmitterConfig cfg;
    cfg.count = 14; cfg.speed = 5.0f; cfg.speedVariance = 1.5f;
    cfg.spread = 0.35f; cfg.lifetime = 0.12f; cfg.lifetimeVariance = 0.03f;
    cfg.colorStart = { 1.0f, 0.95f, 0.20f, 1.0f };
    cfg.colorEnd   = { 1.0f, 0.45f, 0.05f, 0.0f };
    cfg.sizeStart = 10.0f; cfg.sizeEnd = 3.0f; cfg.gravity = 0.0f;
    return cfg;
}

static const DefinitelyEngine::EmitterConfig kBloodHit    = MakeBloodHitConfig();
static const DefinitelyEngine::EmitterConfig kGunMuzzle   = MakeGunMuzzleConfig();
static const DefinitelyEngine::EmitterConfig kPlayerHit   = MakePlayerHitConfig();
static const DefinitelyEngine::EmitterConfig kZombieDeath = MakeZombieDeathConfig();

namespace {
    constexpr const char* kZombieRightHandBoneName = "CC_Base_R_Hand";
    constexpr const char* kPlayerRightHandBoneName = "Hand.R.001";
    constexpr float kAssumedFrameRate = 60.0f;
    constexpr float kAttackHitLogStartFrame = 30.0f;

    float GetClipDurationSeconds(const DefinitelyEngine::AnimationClip* clip) {
        if (!clip || clip->ticksPerSecond <= 0.0f)
            return 0.0f;
        return clip->durationTicks / clip->ticksPerSecond;
    }

    const DefinitelyEngine::BoneNode* FindBoneNodeByName(const DefinitelyEngine::BoneNode& node, const std::string& boneName) {
        if (node.name == boneName)
            return &node;

        for (const auto& child : node.children) {
            if (const auto* found = FindBoneNodeByName(child, boneName))
                return found;
        }

        return nullptr;
    }

    glm::vec3 TransformPoint(const glm::mat4& transform, const glm::vec3& point) {
        return glm::vec3(transform * glm::vec4(point, 1.0f));
    }

    glm::vec3 GetCameraForward(const DefinitelyEngine::PerspectiveCamera& camera) {
        glm::vec3 forward;
        forward.x = glm::cos(glm::radians(camera.GetYaw())) * glm::cos(glm::radians(camera.GetPitch()));
        forward.y = glm::sin(glm::radians(camera.GetPitch()));
        forward.z = glm::sin(glm::radians(camera.GetYaw())) * glm::cos(glm::radians(camera.GetPitch()));
        return glm::normalize(forward);
    }
}

GameLayer::GameLayer()
    : m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f)
{
    m_ZombieModel = new DefinitelyEngine::AnimatedModel("Assets/Models/zombie-with-animations.fbx");
    m_ZombieRunClip    = m_ZombieModel->GetClip("Run");
    m_ZombieAttackClip = m_ZombieModel->GetClip("Attack");
    m_ZombieAttackDuration = GetClipDurationSeconds(m_ZombieAttackClip);
    m_ZombieAttackHitLogStartTime = glm::min(m_ZombieAttackDuration, kAttackHitLogStartFrame / kAssumedFrameRate);

    m_LeftArmModel    = new DefinitelyEngine::AnimatedModel("Assets/Models/mcLeftArm.fbx");
    m_LeftArmAnimator = new DefinitelyEngine::Animator(&m_LeftArmModel->GetSkeleton());
    m_LeftArmIdleClip  = m_LeftArmModel->GetClip("LeftArmIdle");
    m_LeftArmPunchClip = m_LeftArmModel->GetClip("Punch");
    if (m_LeftArmIdleClip) m_LeftArmAnimator->SetClip(m_LeftArmIdleClip);

    m_RightArmModel    = new DefinitelyEngine::AnimatedModel("Assets/Models/mcRightArm.fbx");
    m_RightArmAnimator = new DefinitelyEngine::Animator(&m_RightArmModel->GetSkeleton());
    m_RightArmIdleClip = m_RightArmModel->GetClip("ShootIdle");
    if (m_RightArmIdleClip) m_RightArmAnimator->SetClip(m_RightArmIdleClip);
    m_GunModel         = new DefinitelyEngine::Model("Assets/Models/gun.fbx");

    {
        const DefinitelyEngine::Skeleton& zombieSkeleton = m_ZombieModel->GetSkeleton();
        if (const auto* rightHandBone = FindBoneNodeByName(zombieSkeleton.rootNode, kZombieRightHandBoneName)) {
            DE_TRACE("Zombie bone found at startup: {0}", rightHandBone->name);
        }
    }

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Left Arm";
        obj.viewSpace = true;
        obj.transform.position = { -0.500f, -0.320f, -0.550f };
        obj.transform.rotation = { 0.0f, 73.0f, 0.0f };
        obj.transform.scale    = 0.01f;
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_LeftArmModel->Draw(vp, t, m_LeftArmAnimator->GetBoneMatrices());
        };
        m_Objects.push_back(std::move(obj));
        m_LeftArmObjectIndex = (int)m_Objects.size() - 1;
    }

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Right Arm";
        obj.viewSpace = true;
        obj.transform.position = { 0.700f, -0.200f, -0.700f };
        obj.transform.rotation = { 0.0f, 118.0f, 0.0f };
        obj.transform.scale    = 0.01f;
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_RightArmModel->Draw(vp, t, m_RightArmAnimator->GetBoneMatrices());
        };
        m_Objects.push_back(std::move(obj));
        m_RightArmObjectIndex = (int)m_Objects.size() - 1;
    }

    m_Plane = new Plane();
    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Plane";
        obj.transform.position = { 0.0f, 0.0f, 0.0f };
        obj.transform.scale = 900.0f;
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_Plane->Render(vp, t);
        };
        m_Objects.push_back(std::move(obj));
        m_PlaneObjectIndex = (int)m_Objects.size() - 1;
    }
    m_Objects[m_PlaneObjectIndex].tag = "Ground";
    m_PlaneCollider = new DefinitelyEngine::BoxCollider();
    m_PlaneCollider->ownerName   = "Plane";
    m_PlaneCollider->tag         = "Ground";
    m_PlaneCollider->halfExtents = { 450.0f, 0.05f, 450.0f };
    m_Objects[m_PlaneObjectIndex].collider = m_PlaneCollider;
    m_CollisionWorld.Register(m_PlaneCollider);

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Player";
        obj.transform.position = { 0.0f, 1.6f, 3.0f };
        m_Objects.push_back(std::move(obj));
        m_PlayerObjectIndex = (int)m_Objects.size() - 1;
    }
    m_PlayerCollider = new DefinitelyEngine::CapsuleCollider();
    m_PlayerCollider->ownerName = "Player";
    m_PlayerCollider->tag       = "Player";
    m_PlayerCollider->radius    = 0.3f;
    m_PlayerCollider->height    = 1.4f;
    m_Objects[m_PlayerObjectIndex].collider = m_PlayerCollider;
    m_CollisionWorld.Register(m_PlayerCollider);

    {
        const DefinitelyEngine::Skeleton& rightArmSkeleton = m_RightArmModel->GetSkeleton();
        if (const auto* rightHandBone = FindBoneNodeByName(rightArmSkeleton.rootNode, kPlayerRightHandBoneName)) {
            DE_TRACE("Player right-arm gun attachment bone found at startup: {0}", rightHandBone->name);
        }
    }

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Gun";
        obj.viewSpace = true;
        obj.transform.position = { 0.1f, 0.0f, -0.020f };
        obj.transform.rotation = { -101.0f, -90.0f, 0.0f };
        obj.transform.scale    = 0.01f;
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            if (!m_GunModel)
                return;

            const glm::mat4 rightArmTransform = m_Objects[m_RightArmObjectIndex].transform.GetMatrix();
            glm::mat4 handBoneTransform(1.0f);
            if (m_RightArmAnimator
                && m_RightArmAnimator->TryGetNodeGlobalTransform(kPlayerRightHandBoneName, handBoneTransform)) {
                m_GunModel->Draw(vp, rightArmTransform * handBoneTransform * t);
            } else {
                m_GunModel->Draw(vp, rightArmTransform * t);
            }
        };
        m_Objects.push_back(std::move(obj));
        m_GunObjectIndex = (int)m_Objects.size() - 1;
    }

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "GunMuzzle";
        obj.viewSpace = true;
        obj.transform.position = { 12.2f, 9.1f, -7.5f };
        obj.transform.rotation = { 0.0f, 0.0f, 0.0f };
        obj.transform.scale    = 1.0f;
        m_Objects.push_back(std::move(obj));
        m_GunMuzzleObjectIndex = (int)m_Objects.size() - 1;
    }

    m_CollisionWorld.IgnoreCollision("ZombieHandTrigger", "Enemy");
    m_CollisionWorld.IgnoreCollision("ZombieHandTrigger", "Ground");

    m_DebugDraw = new DefinitelyEngine::DebugDraw();
    m_Particles = new DefinitelyEngine::ParticleSystem(2000);
    StartNextWave();
}

GameLayer::~GameLayer() {
    delete m_SuzanneModel;
    delete m_GunModel;
    delete m_Plane;
    delete m_ZombieModel;
    delete m_LeftArmAnimator;
    delete m_LeftArmModel;
    delete m_RightArmAnimator;
    delete m_RightArmModel;
    delete m_PlaneCollider;
    delete m_PlayerCollider;
    delete m_DebugDraw;
    delete m_Particles;
}

void GameLayer::SpawnZombiesInCircle(int count) {
    if (count <= 0)
        return;

    const glm::vec3 center = m_Camera.GetCamera().GetPosition();
    const float radius = kWaveSpawnRadius;
    const float angleStep = glm::two_pi<float>() / (float)count;

    for (int i = 0; i < count; ++i) {
        const float angle = angleStep * (float)i;
        glm::vec3 pos = {
            center.x + glm::cos(angle) * radius,
            0.0f,
            center.z + glm::sin(angle) * radius
        };

        auto zombie = std::make_unique<Zombie>(
            m_ZombieModel,
            m_ZombieRunClip,
            m_ZombieAttackClip,
            m_ZombieAttackDuration,
            m_ZombieAttackHitLogStartTime,
            pos,
            std::to_string(i));
        zombie->AddToScene(m_Objects);
        m_Zombies.push_back(std::move(zombie));
        m_Zombies.back()->RegisterColliders(m_CollisionWorld, m_Objects);
    }
}

int GameLayer::CountAliveZombies() const {
    int alive = 0;
    for (const auto& zombie : m_Zombies) {
        if (zombie->IsAlive())
            ++alive;
    }
    return alive;
}

void GameLayer::StartNextWave() {
    ++m_CurrentWave;
    const int spawnCount = glm::min(30, 1 << glm::min(m_CurrentWave, 5));
    m_ZombieDamagePerHit = glm::min(60.0f, 30.0f + (float)(m_CurrentWave - 1) * 5.0f);
    m_WaveCountdownActive = false;
    m_NextWaveTimer = 0.0f;
    m_WaveBannerTimer = kWaveBannerDuration;
    SpawnZombiesInCircle(spawnCount);
}

Zombie* GameLayer::FindZombieByCollider(const DefinitelyEngine::Collider* collider) {
    for (const auto& zombie : m_Zombies) {
        if (zombie->OwnsCollider(collider))
            return zombie.get();
    }
    return nullptr;
}

void GameLayer::OnUpdate(float dt) {
    if (m_WaveBannerTimer > 0.0f)
        m_WaveBannerTimer = glm::max(0.0f, m_WaveBannerTimer - dt);

    for (const auto& zombie : m_Zombies)
        zombie->UpdateAnimation(dt);

    if (m_Particles) m_Particles->Update(dt);

    const bool isLmbPressed = DefinitelyEngine::Input::IsMouseButtonPressed(DE_MOUSE_BUTTON_LEFT);
    const bool lmbJustPressed = isLmbPressed && !m_WasLmbPressed;
    m_WasLmbPressed = isLmbPressed;

    if (lmbJustPressed) {
        m_RightArmRecoilZ = 0.15f;
        m_Camera.TriggerShotShake();

        const DefinitelyEngine::PerspectiveCamera& playerCamera = m_Camera.GetCamera();
        if (m_Particles && m_GunMuzzleObjectIndex >= 0) {
            const glm::mat4 rightArmTransform = m_Objects[m_RightArmObjectIndex].transform.GetMatrix();
            glm::mat4 handBoneTransform(1.0f);
            if (!(m_RightArmAnimator
                && m_RightArmAnimator->TryGetNodeGlobalTransform(kPlayerRightHandBoneName, handBoneTransform))) {
                handBoneTransform = glm::mat4(1.0f);
            }

            const glm::mat4 gunViewTransform =
                rightArmTransform
                * handBoneTransform
                * m_Objects[m_GunObjectIndex].transform.GetMatrix()
                * m_Objects[m_GunMuzzleObjectIndex].transform.GetMatrix();
            const glm::vec3 muzzleViewPos = TransformPoint(gunViewTransform, glm::vec3(0.0f));
            const glm::mat4 inverseView = glm::inverse(playerCamera.GetViewMatrix());
            const glm::vec3 muzzleWorldPos = TransformPoint(inverseView, muzzleViewPos);
            m_Particles->Emit(muzzleWorldPos, GetCameraForward(playerCamera), kGunMuzzle);
        }

        DefinitelyEngine::Ray shotRay = { playerCamera.GetPosition(), GetCameraForward(playerCamera) };
        DefinitelyEngine::RaycastHit shotHit;
        if (m_CollisionWorld.Raycast(shotRay, 100.0f, shotHit) && shotHit.tag == "Enemy") {
            if (Zombie* zombie = FindZombieByCollider(shotHit.collider)) {
                zombie->ApplyShotDamage(
                    40.0f,
                    m_CollisionWorld,
                    m_Objects,
                    m_Particles,
                    kBloodHit,
                    kZombieDeath,
                    shotHit,
                    -GetCameraForward(playerCamera));
            }
        }
    }

    m_RightArmRecoilZ = glm::mix(m_RightArmRecoilZ, 0.0f, 1.0f - glm::exp(-20.0f * dt));
    m_Objects[m_RightArmObjectIndex].transform.position.z = -0.700f + m_RightArmRecoilZ;

    const bool isRightMousePressed = DefinitelyEngine::Input::IsMouseButtonPressed(DE_MOUSE_BUTTON_RIGHT);
    const bool leftMouseJustPressed = isRightMousePressed && !m_WasLeftMousePressed;
    m_WasLeftMousePressed = isRightMousePressed;

    if (leftMouseJustPressed && m_LeftArmAnimator && m_LeftArmPunchClip) {
        m_LeftArmAnimator->SetClip(m_LeftArmPunchClip, false);
        m_LeftArmPunchTimeRemaining = GetClipDurationSeconds(m_LeftArmPunchClip);
        m_RightArmTargetRotZ = -90.0f;
        m_RightArmShootTimer = 0.3f;
    }

    if (m_RightArmShootTimer > 0.0f) {
        m_RightArmShootTimer -= dt;
        if (m_RightArmShootTimer <= 0.0f) {
            m_RightArmTargetRotZ = 0.0f;
            m_RightArmShootTimer = 0.0f;
        }
    }

    m_Objects[m_RightArmObjectIndex].transform.rotation.z = glm::mix(
        m_Objects[m_RightArmObjectIndex].transform.rotation.z, m_RightArmTargetRotZ, 1.0f - glm::exp(-15.0f * dt));

    if (m_LeftArmPunchTimeRemaining > 0.0f) {
        m_LeftArmPunchTimeRemaining -= dt;
        if (m_LeftArmPunchTimeRemaining <= 0.0f && m_LeftArmAnimator && m_LeftArmIdleClip) {
            m_LeftArmAnimator->SetClip(m_LeftArmIdleClip);
            m_LeftArmPunchTimeRemaining = 0.0f;
        }
    }

    if (m_LeftArmAnimator)  m_LeftArmAnimator->Update(dt);
    if (m_RightArmAnimator) m_RightArmAnimator->Update(dt);

    m_Camera.OnUpdate(dt);

    {
        glm::vec3 camPos = m_Camera.GetCamera().GetPosition();
        float feetY = camPos.y - kPlayerEyeHeight;

        DefinitelyEngine::Ray downRay = { { camPos.x, feetY + 0.1f, camPos.z }, { 0.0f, -1.0f, 0.0f } };
        DefinitelyEngine::RaycastHit hit;
        bool onGround = m_CollisionWorld.Raycast(downRay, 0.5f, hit, "Enemy")
                        && hit.tag == "Ground"
                        && m_PlayerVertVelocity <= 0.0f;

        if (onGround) {
            m_PlayerGrounded     = true;
            m_PlayerVertVelocity = 0.0f;
            camPos.y             = hit.point.y + kPlayerEyeHeight;
        } else {
            m_PlayerGrounded     = false;
            m_PlayerVertVelocity = glm::max(m_PlayerVertVelocity + kGravity * dt, -15.0f);
            camPos.y            += m_PlayerVertVelocity * dt;
        }
        m_Camera.GetCamera().SetPosition(camPos);
    }

    {
        const bool isSpacePressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_SPACE);
        if (isSpacePressed && !m_WasSpacePressed && m_PlayerGrounded) {
            m_PlayerVertVelocity = kJumpImpulse;
            m_PlayerGrounded     = false;
        }
        m_WasSpacePressed = isSpacePressed;
    }

    m_Objects[m_PlayerObjectIndex].transform.position = m_Camera.GetCamera().GetPosition();
    if (m_Objects[m_PlayerObjectIndex].collider)
        m_Objects[m_PlayerObjectIndex].collider->worldCenter = m_Objects[m_PlayerObjectIndex].transform.position
                                                             + m_Objects[m_PlayerObjectIndex].collider->localOffset;

    const glm::vec3 playerPos = m_Camera.GetCamera().GetPosition();
    for (const auto& zombie : m_Zombies) {
        zombie->UpdateAI(dt, playerPos, m_Objects, kZombieSpeed, kZombieAttackRange, kZombieAttackCooldownDuration);
        zombie->UpdateGravity(dt, m_CollisionWorld, m_Objects, kGravity);
        zombie->SyncColliders(m_Objects, kZombieRightHandBoneName);
    }

    if (CountAliveZombies() == 0) {
        if (!m_WaveCountdownActive) {
            m_WaveCountdownActive = true;
            m_NextWaveTimer = kWaveCountdownDuration;
        } else {
            m_NextWaveTimer = glm::max(0.0f, m_NextWaveTimer - dt);
            if (m_NextWaveTimer <= 0.0f)
                StartNextWave();
        }
    } else {
        m_WaveCountdownActive = false;
        m_NextWaveTimer = 0.0f;
    }

    for (auto& obj : m_Objects) {
        if (obj.collider)
            obj.collider->worldCenter = obj.transform.position + obj.collider->localOffset;
    }

    m_CollisionWorld.Update();

    for (const auto& contact : m_CollisionWorld.GetContacts()) {
        for (const auto& zombie : m_Zombies)
            m_PlayerHealth = glm::max(0.0f, m_PlayerHealth - zombie->TryApplyPlayerHit(contact, m_PlayerCollider, m_Camera.GetCamera(), m_Particles, kPlayerHit, m_ZombieDamagePerHit));

        bool zombiePlayerBodyContact = false;
        for (const auto& zombie : m_Zombies) {
            zombiePlayerBodyContact =
                (zombie->IsBodyCollider(contact.a) && contact.b == m_PlayerCollider) ||
                (zombie->IsBodyCollider(contact.b) && contact.a == m_PlayerCollider);
            if (zombiePlayerBodyContact)
                break;
        }

        if (contact.a->isTrigger || contact.b->isTrigger)
            continue;
        if (zombiePlayerBodyContact)
            continue;

        glm::vec3 push = contact.normal * (contact.depth * 0.5f);
        for (auto& obj : m_Objects) {
            if (!obj.collider) continue;
            if (obj.collider == contact.a) {
                obj.transform.position += push;
                if (obj.collider == m_PlayerCollider)
                    m_Camera.GetCamera().SetPosition(obj.transform.position);
            } else if (obj.collider == contact.b) {
                obj.transform.position -= push;
                if (obj.collider == m_PlayerCollider)
                    m_Camera.GetCamera().SetPosition(obj.transform.position);
            }
        }
    }

    const DefinitelyEngine::PerspectiveCamera& camera = m_Camera.GetCamera();
    const glm::mat4& vp   = camera.GetViewProjectionMatrix();
    const glm::mat4& proj = camera.GetProjectionMatrix();

    for (auto& obj : m_Objects)
        if (!obj.viewSpace) obj.Render(vp);
    for (auto& obj : m_Objects)
        if (obj.viewSpace)  obj.Render(proj);

    if (m_ShowColliders && m_DebugDraw) {
        for (auto& obj : m_Objects) {
            if (!obj.collider || obj.viewSpace) continue;
            glm::vec3 wPos = obj.transform.position + obj.collider->localOffset;
            if (obj.collider->GetType() == DefinitelyEngine::ColliderType::Box) {
                auto* b = static_cast<DefinitelyEngine::BoxCollider*>(obj.collider);
                m_DebugDraw->DrawBox(wPos, b->halfExtents, { 0.0f, 1.0f, 0.0f, 1.0f });
            } else {
                auto* c = static_cast<DefinitelyEngine::CapsuleCollider*>(obj.collider);
                m_DebugDraw->DrawCapsule(wPos, c->radius, c->height, { 0.0f, 1.0f, 1.0f, 1.0f });
            }
        }
        for (const auto& zombie : m_Zombies) {
            if (zombie->IsAlive() && zombie->GetHandCollider()) {
                m_DebugDraw->DrawCapsule(
                    zombie->GetHandCollider()->worldCenter,
                    zombie->GetHandCollider()->radius,
                    zombie->GetHandCollider()->height,
                    { 1.0f, 0.3f, 0.3f, 1.0f });
            }
        }
        m_DebugDraw->Flush(vp);
    }

    if (m_Particles) m_Particles->Render(vp);

    auto& window = DefinitelyEngine::Application::Get().GetWindow();
    m_HudRenderer.BeginFrame((float)window.GetWidth(), (float)window.GetHeight());
    m_HudRenderer.DrawQuad({ -4.0f, -4.0f, 8.0f, 8.0f }, { 1.0f, 1.0f, 1.0f, 0.95f }, DefinitelyEngine::HudAnchor::Center);
    constexpr DefinitelyEngine::HudRect healthBarRect = { 24.0f, 56.0f, 240.0f, 24.0f };
    m_HudRenderer.DrawQuad({ 20.0f, 52.0f, 248.0f, 32.0f }, { 0.0f, 0.0f, 0.0f, 0.35f }, DefinitelyEngine::HudAnchor::BottomLeft);
    m_HudRenderer.DrawHealthBar(
        healthBarRect,
        m_PlayerMaxHealth > 0.0f ? (m_PlayerHealth / m_PlayerMaxHealth) : 0.0f,
        { 0.08f, 0.08f, 0.08f, 0.9f },
        { 0.85f, 0.12f, 0.12f, 1.0f },
        DefinitelyEngine::HudAnchor::BottomLeft);

    const std::string healthText = "HP " + std::to_string((int)m_PlayerHealth) + "/" + std::to_string((int)m_PlayerMaxHealth);
    const DefinitelyEngine::HudTextStyle healthTextStyle = { 2.0f, { 1.0f, 1.0f, 1.0f, 1.0f } };
    m_HudRenderer.DrawText(healthText, { 30.0f, 24.0f }, healthTextStyle, DefinitelyEngine::HudAnchor::BottomLeft);

    const float frameHeight = (float)window.GetHeight();
    const std::string waveTopText = "WAVE " + std::to_string(m_CurrentWave);
    const DefinitelyEngine::HudTextStyle waveTopStyle = { 2.5f, { 1.0f, 0.95f, 0.75f, 1.0f } };
    m_HudRenderer.DrawText(
        waveTopText,
        { 0.0f, -frameHeight * 0.5f + 22.0f },
        waveTopStyle,
        DefinitelyEngine::HudAnchor::Center);

    if (m_WaveCountdownActive) {
        const std::string nextText = "NEXT " + std::to_string((int)glm::ceil(m_NextWaveTimer));
        const DefinitelyEngine::HudTextStyle nextStyle = { 2.0f, { 1.0f, 1.0f, 1.0f, 0.95f } };
        m_HudRenderer.DrawText(
            nextText,
            { 0.0f, -frameHeight * 0.5f + 46.0f },
            nextStyle,
            DefinitelyEngine::HudAnchor::Center);
    }

    if (m_WaveBannerTimer > 0.0f) {
        const float t = 1.0f - (m_WaveBannerTimer / kWaveBannerDuration);
        const float size = glm::mix(14.0f, 5.0f, t);
        const float alpha = glm::mix(1.0f, 0.0f, t);
        const std::string bannerText = "WAVE " + std::to_string(m_CurrentWave);
        const DefinitelyEngine::HudTextStyle bannerStyle = { size, { 1.0f, 0.95f, 0.40f, alpha } };
        m_HudRenderer.DrawText(
            bannerText,
            { 0.0f, -40.0f },
            bannerStyle,
            DefinitelyEngine::HudAnchor::Center);
    }
    m_HudRenderer.EndFrame();
}

void GameLayer::OnAttach() {}
void GameLayer::OnDetach() {}

void GameLayer::OnEvent(DefinitelyEngine::Event& e) {
    if (e.GetType() == DefinitelyEngine::EventType::WindowResize) {
        auto& resizeEvent = static_cast<DefinitelyEngine::WindowResizeEvent&>(e);
        if (resizeEvent.GetHeight() > 0) {
            m_Camera.GetCamera().SetAspectRatio(
                (float)resizeEvent.GetWidth() / (float)resizeEvent.GetHeight());
        }
    }
}
