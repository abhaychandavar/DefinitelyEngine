#pragma once

#include "DefinitelyEngine.h"
#include "MainCamera.h"
#include "Game/Entities/Plane.h"

class GameLayer : public DefinitelyEngine::Layer {
public:
    GameLayer();
    ~GameLayer();

    void OnUpdate(float dt) override;
    void OnEvent(DefinitelyEngine::Event& e) override;
    void OnAttach() override;
    void OnDetach() override;

    std::vector<DefinitelyEngine::GameObject>& GetObjects() { return m_Objects; }
    int& GetSelectedIndex() { return m_SelectedIndex; }

private:
    std::vector<DefinitelyEngine::GameObject> m_Objects;
    int m_SelectedIndex = -1;

    DefinitelyEngine::Model* m_SuzanneModel = nullptr;
    Plane* m_Plane = nullptr;

    DefinitelyEngine::AnimatedModel* m_ZombieModel    = nullptr;
    DefinitelyEngine::Animator*      m_ZombieAnimator = nullptr;

    const DefinitelyEngine::AnimationClip* m_ZombieRunClip    = nullptr;
    const DefinitelyEngine::AnimationClip* m_ZombieAttackClip = nullptr;

    enum class ZombieState { Follow, Attacking };
    ZombieState m_ZombieState          = ZombieState::Follow;
    float       m_ZombieAttackTimeLeft = 0.0f;
    float       m_ZombieAttackCooldown = 0.0f;
    float       m_ZombieAttackDuration = 0.0f;
    float       m_ZombieAttackHitLogStartTime = 0.0f;

    static constexpr float kZombieSpeed                  = 1.5f;
    static constexpr float kZombieAttackRange            = 0.8f;
    static constexpr float kZombieAttackCooldownDuration = 0.3f;

    DefinitelyEngine::AnimatedModel* m_LeftArmModel     = nullptr;
    DefinitelyEngine::Animator*      m_LeftArmAnimator  = nullptr;
    DefinitelyEngine::AnimatedModel* m_RightArmModel    = nullptr;
    DefinitelyEngine::Animator*      m_RightArmAnimator = nullptr;

    const DefinitelyEngine::AnimationClip* m_LeftArmIdleClip  = nullptr;
    const DefinitelyEngine::AnimationClip* m_LeftArmPunchClip = nullptr;
    const DefinitelyEngine::AnimationClip* m_RightArmIdleClip = nullptr;

    float m_LeftArmPunchTimeRemaining = 0.0f;
    float m_RightArmShootTimer        = 0.0f;
    float m_RightArmTargetRotZ        = 0.0f;
    float m_RightArmRecoilZ           = 0.0f;
    bool  m_WasLeftMousePressed       = false;
    bool  m_WasLmbPressed             = false;

    // Gravity
    static constexpr float kGravity          = -9.8f;
    static constexpr float kJumpImpulse      =  4.5f;
    static constexpr float kPlayerEyeHeight  =  1.6f;

    float m_PlayerVertVelocity = 0.0f;
    bool  m_PlayerGrounded     = false;
    bool  m_WasSpacePressed    = false;

    float m_ZombieVertVelocity = 0.0f;
    bool  m_ZombieGrounded     = false;
    bool  m_ZombieHandHitLoggedThisAttack = false;

    // Collision
    DefinitelyEngine::CollisionWorld   m_CollisionWorld;
    DefinitelyEngine::BoxCollider*     m_PlaneCollider   = nullptr;
    DefinitelyEngine::CapsuleCollider* m_ZombieCollider  = nullptr;
    DefinitelyEngine::CapsuleCollider* m_ZombieHandCollider = nullptr;
    DefinitelyEngine::CapsuleCollider* m_PlayerCollider  = nullptr;
    DefinitelyEngine::DebugDraw*       m_DebugDraw       = nullptr;
    bool                               m_ShowColliders   = true;

    MainCamera m_Camera;
    DefinitelyEngine::HudRenderer m_HudRenderer;

    float m_PlayerHealth    = 100.0f;
    float m_PlayerMaxHealth = 100.0f;
};
