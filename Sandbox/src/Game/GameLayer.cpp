#include "GameLayer.h"
#include "DefinitelyEngine/Input.h"
#include "DefinitelyEngine/Events/WindowResizeEvent.h"

#include <string>

namespace {
    constexpr const char* kZombieRightHandBoneName = "CC_Base_R_Hand";
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

    glm::vec3 ExtractTranslation(const glm::mat4& transform) {
        return glm::vec3(transform[3]);
    }
}

GameLayer::GameLayer()
    : m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 100.0f)
{

    // Zombie — animation test milestone
    // zombie.fbx must be at Assets/Models/zombie.fbx (Mixamo pipeline, see DevNotes/wave-shooter.md).
    // The log output at startup will print the exact clip names loaded from the FBX.
    // Update the SetClip call below once you know the correct name.
    m_ZombieModel    = new DefinitelyEngine::AnimatedModel("Assets/Models/zombie-with-animations.fbx");
    m_ZombieAnimator = new DefinitelyEngine::Animator(&m_ZombieModel->GetSkeleton());
    m_ZombieRunClip    = m_ZombieModel->GetClip("Run");
    m_ZombieAttackClip = m_ZombieModel->GetClip("Attack");
    if (m_ZombieRunClip) m_ZombieAnimator->SetClip(m_ZombieRunClip);
    m_ZombieAttackDuration = GetClipDurationSeconds(m_ZombieAttackClip);
    m_ZombieAttackHitLogStartTime = glm::min(m_ZombieAttackDuration, kAttackHitLogStartFrame / kAssumedFrameRate);
    if (m_ZombieAttackDuration > 0.0f) {
        DE_TRACE("Zombie attack length: {0:.3f}s, hand-hit log starts at {1:.3f}s (frame {2:.0f} @ {3:.0f} fps)",
            m_ZombieAttackDuration,
            m_ZombieAttackHitLogStartTime,
            kAttackHitLogStartFrame,
            kAssumedFrameRate);
    }

    {
        const DefinitelyEngine::Skeleton& zombieSkeleton = m_ZombieModel->GetSkeleton();
        if (const auto* rightHandBone = FindBoneNodeByName(zombieSkeleton.rootNode, kZombieRightHandBoneName)) {
            DE_TRACE("Zombie bone found at startup: {0}", rightHandBone->name);
        }
    }

    m_LeftArmModel    = new DefinitelyEngine::AnimatedModel("Assets/Models/mcLeftArm.fbx");
    m_LeftArmAnimator = new DefinitelyEngine::Animator(&m_LeftArmModel->GetSkeleton());
    m_LeftArmIdleClip  = m_LeftArmModel->GetClip("LeftArmIdle");
    m_LeftArmPunchClip = m_LeftArmModel->GetClip("Punch");
    if (m_LeftArmIdleClip) m_LeftArmAnimator->SetClip(m_LeftArmIdleClip);

    m_RightArmModel    = new DefinitelyEngine::AnimatedModel("Assets/Models/mcRightArm.fbx");
    m_RightArmAnimator = new DefinitelyEngine::Animator(&m_RightArmModel->GetSkeleton());
    m_RightArmIdleClip = m_RightArmModel->GetClip("ShootIdle");
    if (m_RightArmIdleClip) m_RightArmAnimator->SetClip(m_RightArmIdleClip);

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Zombie";
        obj.transform.position = { 2.0f, 0.0f, 0.0f };
        obj.transform.scale    = 0.01f;  // Mixamo exports in centimetres
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_ZombieModel->Draw(vp, t, m_ZombieAnimator->GetBoneMatrices());
        };
        m_Objects.push_back(std::move(obj));
    }
    m_ZombieCollider = new DefinitelyEngine::CapsuleCollider();
    m_ZombieCollider->ownerName   = "Zombie";
    m_ZombieCollider->tag         = "Enemy";
    m_ZombieCollider->localOffset = { 0.010f, 0.270f, 0.000f };
    m_ZombieCollider->radius      = 0.3f;
    m_ZombieCollider->height      = 1.2f;
    m_Objects[0].collider = m_ZombieCollider;
    m_CollisionWorld.Register(m_ZombieCollider);

    m_ZombieHandCollider = new DefinitelyEngine::CapsuleCollider();
    m_ZombieHandCollider->ownerName = "ZombieRightHandTrigger";
    m_ZombieHandCollider->tag       = "ZombieHandTrigger";
    m_ZombieHandCollider->isTrigger = true;
    m_ZombieHandCollider->radius    = 0.18f;
    m_ZombieHandCollider->height    = 0.02f;
    m_CollisionWorld.Register(m_ZombieHandCollider);
    m_CollisionWorld.IgnoreCollision("ZombieHandTrigger", "Enemy");
    m_CollisionWorld.IgnoreCollision("ZombieHandTrigger", "Ground");

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Left Arm";
        obj.viewSpace = true;
        // View-space transform: position is camera-local offset (right, up, -forward).
        // Rotation bakes in the base -90° yaw offset (model forward vs view -Z) plus the local tweak.
        obj.transform.position = { -0.500f, -0.320f, -0.550f };
        obj.transform.rotation = {   0.0f,  73.0f,    0.0f };
        obj.transform.scale    = 0.01f;  // FBX exports in centimetres, same as zombie
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_LeftArmModel->Draw(vp, t, m_LeftArmAnimator->GetBoneMatrices());
        };
        m_Objects.push_back(std::move(obj));
    }

    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Right Arm";
        obj.viewSpace = true;
        obj.transform.position = { 0.700f, -0.200f, -0.700f };
        obj.transform.rotation = {  0.0f,  118.0f,   0.0f };
        obj.transform.scale    = 0.01f;  // FBX exports in cm, same as zombie
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_RightArmModel->Draw(vp, t, m_RightArmAnimator->GetBoneMatrices());
        };
        m_Objects.push_back(std::move(obj));
    }

    // Plane
    m_Plane = new Plane();
    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Plane";
        obj.transform.position = { 0.0f, 0.0f, 0.0f };
        obj.transform.scale = 45.0f;
        obj.onRender = [this](const glm::mat4& vp, const glm::mat4& t) {
            m_Plane->Render(vp, t);
        };
        m_Objects.push_back(std::move(obj));
    }
    m_Objects[3].tag     = "Ground";
    DE_TRACE("Ground plane — position: ({0:.2f}, {1:.2f}, {2:.2f})  scale: {3:.2f}",
        m_Objects[3].transform.position.x,
        m_Objects[3].transform.position.y,
        m_Objects[3].transform.position.z,
        m_Objects[3].transform.scale);
    m_PlaneCollider = new DefinitelyEngine::BoxCollider();
    m_PlaneCollider->ownerName   = "Plane";
    m_PlaneCollider->tag         = "Ground";
    m_PlaneCollider->halfExtents = { 22.5f, 0.05f, 22.5f };  // matches visual scale 45
    m_Objects[3].collider = m_PlaneCollider;
    m_CollisionWorld.Register(m_PlaneCollider);

    // Player (invisible, tracks camera position)
    {
        DefinitelyEngine::GameObject obj;
        obj.name = "Player";
        obj.transform.position = { 0.0f, 1.6f, 3.0f };
        m_Objects.push_back(std::move(obj));
    }
    m_PlayerCollider = new DefinitelyEngine::CapsuleCollider();
    m_PlayerCollider->ownerName = "Player";
    m_PlayerCollider->tag       = "Player";
    m_PlayerCollider->radius    = 0.3f;
    m_PlayerCollider->height    = 1.4f;
    m_Objects[4].collider = m_PlayerCollider;
    m_CollisionWorld.Register(m_PlayerCollider);

    m_DebugDraw = new DefinitelyEngine::DebugDraw();
}

GameLayer::~GameLayer() {
    delete m_SuzanneModel;
    delete m_Plane;
    delete m_ZombieAnimator;
    delete m_ZombieModel;
    delete m_LeftArmAnimator;
    delete m_LeftArmModel;
    delete m_RightArmAnimator;
    delete m_RightArmModel;
    delete m_PlaneCollider;
    delete m_ZombieCollider;
    delete m_ZombieHandCollider;
    delete m_PlayerCollider;
    delete m_DebugDraw;
}

void GameLayer::OnUpdate(float dt) {
    // Advance zombie animation
    if (m_ZombieAnimator) m_ZombieAnimator->Update(dt);

    const bool isLmbPressed = DefinitelyEngine::Input::IsMouseButtonPressed(DE_MOUSE_BUTTON_LEFT);
    const bool lmbJustPressed = isLmbPressed && !m_WasLmbPressed;
    m_WasLmbPressed = isLmbPressed;

    if (lmbJustPressed)
        m_RightArmRecoilZ = 0.15f;  // knock back toward camera

    // Lerp recoil back to 0
    m_RightArmRecoilZ = glm::mix(m_RightArmRecoilZ, 0.0f, 1.0f - glm::exp(-20.0f * dt));
    m_Objects[2].transform.position.z = -0.700f + m_RightArmRecoilZ;

    const bool isRightMousePressed = DefinitelyEngine::Input::IsMouseButtonPressed(DE_MOUSE_BUTTON_RIGHT);
    const bool leftMouseJustPressed = isRightMousePressed && !m_WasLeftMousePressed;
    m_WasLeftMousePressed = isRightMousePressed;

    if (leftMouseJustPressed && m_LeftArmAnimator && m_LeftArmPunchClip) {
        m_LeftArmAnimator->SetClip(m_LeftArmPunchClip, false);
        m_LeftArmPunchTimeRemaining = GetClipDurationSeconds(m_LeftArmPunchClip);

        // Tilt right arm on shoot; restore after 0.3 sec
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

    // Lerp right arm Z rotation toward target
    m_Objects[2].transform.rotation.z = glm::mix(
        m_Objects[2].transform.rotation.z, m_RightArmTargetRotZ, 1.0f - glm::exp(-15.0f * dt));

    if (m_LeftArmPunchTimeRemaining > 0.0f) {
        m_LeftArmPunchTimeRemaining -= dt;
        if (m_LeftArmPunchTimeRemaining <= 0.0f && m_LeftArmAnimator && m_LeftArmIdleClip) {
            m_LeftArmAnimator->SetClip(m_LeftArmIdleClip);
            m_LeftArmPunchTimeRemaining = 0.0f;
        }
    }

    if (m_LeftArmAnimator)  m_LeftArmAnimator->Update(dt);
    if (m_RightArmAnimator) m_RightArmAnimator->Update(dt);

    m_Camera.OnUpdate(dt);  // XZ movement only (flat front vector, no Y change)

    if (m_ZombieHandCollider && m_ZombieAnimator) {
        glm::mat4 handBoneTransform(1.0f);
        if (m_ZombieAnimator->TryGetNodeGlobalTransform(kZombieRightHandBoneName, handBoneTransform)) {
            const glm::mat4 zombieWorld = m_Objects[0].transform.GetMatrix();
            const glm::vec3 handWorldPosition = ExtractTranslation(zombieWorld * handBoneTransform);
            m_ZombieHandCollider->worldCenter = handWorldPosition;
        } else {
            m_ZombieHandCollider->worldCenter = m_Objects[0].transform.position;
        }
    }

    // --- Update worldCenters for all colliders (plane + zombie) ---
    for (auto& obj : m_Objects) {
        if (obj.collider)
            obj.collider->worldCenter = obj.transform.position + obj.collider->localOffset;
    }

    // --- Player gravity + ground check ---
    {
        glm::vec3 camPos = m_Camera.GetCamera().GetPosition();
        float feetY = camPos.y - kPlayerEyeHeight;

        DefinitelyEngine::Ray downRay = { { camPos.x, feetY + 0.1f, camPos.z }, { 0.0f, -1.0f, 0.0f } };
        DefinitelyEngine::RaycastHit hit;
        bool onGround = m_CollisionWorld.Raycast(downRay, 0.5f, hit)
                        && hit.tag == "Ground"
                        && m_PlayerVertVelocity <= 0.0f;

        if (onGround) {
            if (!m_PlayerGrounded)
                DE_TRACE("Player landed on ground at y={0:.3f}", hit.point.y);
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

    // --- Jump (spacebar, player only) ---
    {
        const bool isSpacePressed = DefinitelyEngine::Input::IsKeyPressed(DE_KEY_SPACE);
        if (isSpacePressed && !m_WasSpacePressed && m_PlayerGrounded) {
            m_PlayerVertVelocity = kJumpImpulse;
            m_PlayerGrounded     = false;
        }
        m_WasSpacePressed = isSpacePressed;
    }

    // --- Sync Player object + update its collider worldCenter ---
    m_Objects[4].transform.position = m_Camera.GetCamera().GetPosition();
    if (m_Objects[4].collider)
        m_Objects[4].collider->worldCenter = m_Objects[4].transform.position
                                           + m_Objects[4].collider->localOffset;

    // --- Zombie AI ---
    {
        glm::vec3& zombiePos = m_Objects[0].transform.position;
        glm::vec3  playerPos = m_Camera.GetCamera().GetPosition();

        float dx     = playerPos.x - zombiePos.x;
        float dz     = playerPos.z - zombiePos.z;
        float xzDist = glm::sqrt(dx * dx + dz * dz);

        // Always face the player
        if (xzDist > 0.01f)
            m_Objects[0].transform.rotation.y = glm::degrees(glm::atan(dx, dz));

        const bool inAttackRange = xzDist < kZombieAttackRange;

        if (m_ZombieState == ZombieState::Attacking) {
            if (!inAttackRange) {
                m_ZombieState = ZombieState::Follow;
                m_ZombieAttackTimeLeft = 0.0f;
                m_ZombieAttackCooldown = 0.0f;
                m_ZombieHandHitLoggedThisAttack = false;
                if (m_ZombieRunClip) m_ZombieAnimator->SetClip(m_ZombieRunClip);
            } else if (m_ZombieAttackTimeLeft > 0.0f) {
                m_ZombieAttackTimeLeft -= dt;
                if (m_ZombieAttackTimeLeft <= 0.0f) {
                    m_ZombieAttackTimeLeft = 0.0f;
                    m_ZombieAttackCooldown = kZombieAttackCooldownDuration;
                    m_ZombieHandHitLoggedThisAttack = false;
                }
            } else {
                if (m_ZombieAttackCooldown > 0.0f)
                    m_ZombieAttackCooldown -= dt;

                if (m_ZombieAttackCooldown <= 0.0f && m_ZombieAttackClip) {
                    m_ZombieAttackCooldown = 0.0f;
                    m_ZombieAttackTimeLeft = m_ZombieAttackDuration;
                    m_ZombieHandHitLoggedThisAttack = false;
                    m_ZombieAnimator->SetClip(m_ZombieAttackClip, false);
                    DE_TRACE("Zombie attacks! distance={0:.2f}", xzDist);
                }
            }
        } else {
            if (m_ZombieAttackCooldown > 0.0f)
                m_ZombieAttackCooldown -= dt;

            if (inAttackRange && m_ZombieAttackCooldown <= 0.0f && m_ZombieAttackClip) {
                // Start attack
                m_ZombieState          = ZombieState::Attacking;
                m_ZombieAttackTimeLeft = m_ZombieAttackDuration;
                m_ZombieAttackCooldown = 0.0f;
                m_ZombieHandHitLoggedThisAttack = false;
                m_ZombieAnimator->SetClip(m_ZombieAttackClip, false);
                DE_TRACE("Zombie attacks! distance={0:.2f}", xzDist);
            } else if (xzDist > 0.01f) {
                // Move toward player (XZ only — gravity handles Y)
                float invDist = 1.0f / xzDist;
                zombiePos.x  += dx * invDist * kZombieSpeed * dt;
                zombiePos.z  += dz * invDist * kZombieSpeed * dt;
            }
        }
    }

    // --- Zombie gravity + ground check ---
    {
        glm::vec3& zombiePos = m_Objects[0].transform.position;

        DefinitelyEngine::Ray downRay = { { zombiePos.x, zombiePos.y + 0.1f, zombiePos.z }, { 0.0f, -1.0f, 0.0f } };
        DefinitelyEngine::RaycastHit hit;
        bool onGround = m_CollisionWorld.Raycast(downRay, 0.5f, hit)
                        && hit.tag == "Ground"
                        && m_ZombieVertVelocity <= 0.0f;

        if (onGround) {
            if (!m_ZombieGrounded)
                DE_TRACE("Zombie landed on ground at y={0:.3f}", hit.point.y);
            m_ZombieGrounded     = true;
            m_ZombieVertVelocity = 0.0f;
            zombiePos.y          = hit.point.y;
        } else {
            m_ZombieGrounded     = false;
            m_ZombieVertVelocity = glm::max(m_ZombieVertVelocity + kGravity * dt, -15.0f);
            zombiePos.y         += m_ZombieVertVelocity * dt;
        }
    }

    m_CollisionWorld.Update();

    // --- Resolve contacts: push overlapping capsules apart ---
    for (const auto& contact : m_CollisionWorld.GetContacts()) {
        const bool isZombieHandPlayerContact =
            (contact.a == m_ZombieHandCollider && contact.b == m_PlayerCollider) ||
            (contact.a == m_PlayerCollider && contact.b == m_ZombieHandCollider);
        const float zombieAttackElapsedTime = m_ZombieAttackDuration - m_ZombieAttackTimeLeft;
        const bool isPastAttackHitLogStart =
            zombieAttackElapsedTime >= m_ZombieAttackHitLogStartTime;
        if (isZombieHandPlayerContact
            && m_ZombieState == ZombieState::Attacking
            && isPastAttackHitLogStart
            && !m_ZombieHandHitLoggedThisAttack) {
            DE_TRACE("Zombie hand trigger intersected player");
            m_PlayerHealth = glm::max(0.0f, m_PlayerHealth - 10.0f);
            m_ZombieHandHitLoggedThisAttack = true;
        }

        if (contact.a->isTrigger || contact.b->isTrigger)
            continue;

        glm::vec3 push = contact.normal * (contact.depth * 0.5f);

        for (auto& obj : m_Objects) {
            if (!obj.collider) continue;
            if (obj.collider == contact.a) {
                obj.transform.position += push;
                // If this is the player, keep camera in sync
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

    // World-space objects first, then view-space (arms) on top.
    for (auto& obj : m_Objects)
        if (!obj.viewSpace) obj.Render(vp);
    for (auto& obj : m_Objects)
        if (obj.viewSpace)  obj.Render(proj);

    // Debug collider visualization
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
        if (m_ZombieHandCollider) {
            m_DebugDraw->DrawCapsule(
                m_ZombieHandCollider->worldCenter,
                m_ZombieHandCollider->radius,
                m_ZombieHandCollider->height,
                { 1.0f, 0.3f, 0.3f, 1.0f });
        }
        m_DebugDraw->Flush(vp);
    }

    auto& window = DefinitelyEngine::Application::Get().GetWindow();
    m_HudRenderer.BeginFrame((float)window.GetWidth(), (float)window.GetHeight());

    m_HudRenderer.DrawQuad(
        { -4.0f, -4.0f, 8.0f, 8.0f },
        { 1.0f, 1.0f, 1.0f, 0.95f },
        DefinitelyEngine::HudAnchor::Center);

    constexpr DefinitelyEngine::HudRect healthBarRect = { 24.0f, 56.0f, 240.0f, 24.0f };
    m_HudRenderer.DrawQuad(
        { 20.0f, 52.0f, 248.0f, 32.0f },
        { 0.0f, 0.0f, 0.0f, 0.35f },
        DefinitelyEngine::HudAnchor::BottomLeft);

    m_HudRenderer.DrawHealthBar(
        healthBarRect,
        m_PlayerMaxHealth > 0.0f ? (m_PlayerHealth / m_PlayerMaxHealth) : 0.0f,
        { 0.08f, 0.08f, 0.08f, 0.9f },
        { 0.85f, 0.12f, 0.12f, 1.0f },
        DefinitelyEngine::HudAnchor::BottomLeft);

    const std::string healthText =
        "HP " + std::to_string((int)m_PlayerHealth) + "/" + std::to_string((int)m_PlayerMaxHealth);
    const DefinitelyEngine::HudTextStyle healthTextStyle = {
        2.0f,
        { 1.0f, 1.0f, 1.0f, 1.0f }
    };
    m_HudRenderer.DrawText(
        healthText,
        { 30.0f, 24.0f },
        healthTextStyle,
        DefinitelyEngine::HudAnchor::BottomLeft);

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
