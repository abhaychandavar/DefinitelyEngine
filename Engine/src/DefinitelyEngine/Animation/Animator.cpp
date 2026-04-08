#include "depch.h"
#include "Animator.h"

namespace DefinitelyEngine {

    Animator::Animator(const Skeleton* skeleton)
        : m_Skeleton(skeleton)
    {
        if (m_Skeleton)
            m_BoneMatrices.assign(m_Skeleton->boneCount, glm::mat4(1.0f));
    }

    void Animator::SetClip(const AnimationClip* clip, bool loop) {
        m_Clip        = clip;
        m_CurrentTime = 0.0f;
        m_Loop        = loop;
    }

    void Animator::Update(float dt) {
        if (!m_Clip || !m_Skeleton) return;

        // Advance time (in ticks), loop or clamp depending on m_Loop
        m_CurrentTime += dt * m_Clip->ticksPerSecond;
        if (m_Loop)
            m_CurrentTime = std::fmod(m_CurrentTime, m_Clip->durationTicks);
        else
            m_CurrentTime = std::min(m_CurrentTime, m_Clip->durationTicks);

        // Reset to identity before traversal so uninfluenced bones stay in bind pose
        m_BoneMatrices.assign(m_Skeleton->boneCount, glm::mat4(1.0f));
        m_NodeGlobalTransforms.clear();

        ComputeBoneTransforms(m_Skeleton->rootNode, glm::mat4(1.0f));
    }

    bool Animator::TryGetNodeGlobalTransform(const std::string& nodeName, glm::mat4& outTransform) const {
        auto it = m_NodeGlobalTransforms.find(nodeName);
        if (it == m_NodeGlobalTransforms.end())
            return false;

        outTransform = it->second;
        return true;
    }

    // -------------------------------------------------------------------------

    void Animator::ComputeBoneTransforms(const BoneNode& node, const glm::mat4& parentTransform) {
        glm::mat4 nodeTransform = node.localTransform;

        // If this bone has a channel in the current clip, override with interpolated pose
        auto it = m_Clip->channelMap.find(node.name);
        if (it != m_Clip->channelMap.end()) {
            const BoneChannel& ch = m_Clip->channels[it->second];

            glm::vec3 pos   = InterpolatePosition(ch, m_CurrentTime);
            glm::quat rot   = InterpolateRotation(ch, m_CurrentTime);
            glm::vec3 scale = InterpolateScale   (ch, m_CurrentTime);

            nodeTransform = glm::translate(glm::mat4(1.0f), pos)
                          * glm::mat4_cast(rot)
                          * glm::scale(glm::mat4(1.0f), scale);
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;
        m_NodeGlobalTransforms[node.name] = globalTransform;

        // Write final bone matrix if this node corresponds to a bone
        auto boneIt = m_Skeleton->boneInfoMap.find(node.name);
        if (boneIt != m_Skeleton->boneInfoMap.end()) {
            const BoneInfo& info = boneIt->second;
            m_BoneMatrices[info.id] = m_Skeleton->globalInverseTransform
                                    * globalTransform
                                    * info.offsetMatrix;
        }

        for (const BoneNode& child : node.children)
            ComputeBoneTransforms(child, globalTransform);
    }

    // -------------------------------------------------------------------------
    // Position interpolation

    int Animator::FindPositionIndex(const BoneChannel& ch, float time) {
        for (int i = 0; i < (int)ch.posKeys.size() - 1; i++)
            if (time < ch.posKeys[i + 1].time) return i;
        return (int)ch.posKeys.size() - 2;
    }

    glm::vec3 Animator::InterpolatePosition(const BoneChannel& ch, float time) {
        if (ch.posKeys.size() == 1) return ch.posKeys[0].value;
        int i = FindPositionIndex(ch, time);
        float t = (time - ch.posKeys[i].time) /
                  (ch.posKeys[i + 1].time - ch.posKeys[i].time);
        return glm::mix(ch.posKeys[i].value, ch.posKeys[i + 1].value, t);
    }

    // -------------------------------------------------------------------------
    // Rotation interpolation

    int Animator::FindRotationIndex(const BoneChannel& ch, float time) {
        for (int i = 0; i < (int)ch.rotKeys.size() - 1; i++)
            if (time < ch.rotKeys[i + 1].time) return i;
        return (int)ch.rotKeys.size() - 2;
    }

    glm::quat Animator::InterpolateRotation(const BoneChannel& ch, float time) {
        if (ch.rotKeys.size() == 1) return ch.rotKeys[0].value;
        int i = FindRotationIndex(ch, time);
        float t = (time - ch.rotKeys[i].time) /
                  (ch.rotKeys[i + 1].time - ch.rotKeys[i].time);
        return glm::normalize(glm::slerp(ch.rotKeys[i].value, ch.rotKeys[i + 1].value, t));
    }

    // -------------------------------------------------------------------------
    // Scale interpolation

    int Animator::FindScaleIndex(const BoneChannel& ch, float time) {
        for (int i = 0; i < (int)ch.scaleKeys.size() - 1; i++)
            if (time < ch.scaleKeys[i + 1].time) return i;
        return (int)ch.scaleKeys.size() - 2;
    }

    glm::vec3 Animator::InterpolateScale(const BoneChannel& ch, float time) {
        if (ch.scaleKeys.size() == 1) return ch.scaleKeys[0].value;
        int i = FindScaleIndex(ch, time);
        float t = (time - ch.scaleKeys[i].time) /
                  (ch.scaleKeys[i + 1].time - ch.scaleKeys[i].time);
        return glm::mix(ch.scaleKeys[i].value, ch.scaleKeys[i + 1].value, t);
    }

}
