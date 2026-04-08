#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"
#include "Skeleton.h"
#include "AnimationClip.h"

namespace DefinitelyEngine {

    // Animator is a standalone runtime component — it does NOT know about AnimatedModel
    // or any rendering. It takes a Skeleton* (read-only) and an AnimationClip* (read-only),
    // advances time each frame, and produces a flat array of bone matrices ready to upload.
    //
    // AnimatorState (to be added later) wraps Animator and decides which clip to play.
    // Blend trees composite multiple Animators and blend their GetBoneMatrices() results.
    // Neither of those requires changing this class.
    class DEFINITELY_ENGINE_API Animator {
    public:
        explicit Animator(const Skeleton* skeleton);

        // Select clip to play; resets playback time to 0.
        // Pass loop=false for one-shot clips (time clamps at the last frame).
        void SetClip(const AnimationClip* clip, bool loop = true);

        // Advance animation by dt seconds and recompute all bone matrices.
        // Safe to call with no clip set — produces identity matrices.
        void Update(float dt);

        const std::vector<glm::mat4>& GetBoneMatrices() const { return m_BoneMatrices; }
        bool TryGetNodeGlobalTransform(const std::string& nodeName, glm::mat4& outTransform) const;

    private:
        const Skeleton*        m_Skeleton    = nullptr;
        const AnimationClip*   m_Clip        = nullptr;
        float                  m_CurrentTime = 0.0f;   // in ticks
        bool                   m_Loop        = true;
        std::vector<glm::mat4> m_BoneMatrices;
        std::unordered_map<std::string, glm::mat4> m_NodeGlobalTransforms;

        // Recursive hierarchy traversal — fills m_BoneMatrices
        void ComputeBoneTransforms(const BoneNode& node, const glm::mat4& parentTransform);

        // Keyframe interpolation helpers
        glm::vec3 InterpolatePosition(const BoneChannel& ch, float time);
        glm::quat InterpolateRotation(const BoneChannel& ch, float time);
        glm::vec3 InterpolateScale   (const BoneChannel& ch, float time);

        int FindPositionIndex(const BoneChannel& ch, float time);
        int FindRotationIndex(const BoneChannel& ch, float time);
        int FindScaleIndex   (const BoneChannel& ch, float time);
    };

}
