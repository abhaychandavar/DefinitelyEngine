#pragma once

#include "depch.h"

namespace DefinitelyEngine {

    struct KeyPosition {
        float     time;
        glm::vec3 value;
    };

    struct KeyRotation {
        float     time;
        glm::quat value;
    };

    struct KeyScale {
        float     time;
        glm::vec3 value;
    };

    // All keyframe channels for a single bone within one animation clip.
    struct BoneChannel {
        std::string              boneName;
        std::vector<KeyPosition> posKeys;
        std::vector<KeyRotation> rotKeys;
        std::vector<KeyScale>    scaleKeys;
    };

    // A single named animation (e.g. "Walk", "Attack", "Death").
    // Pure data — owns no GPU resources.
    struct AnimationClip {
        std::string                          name;
        float                                durationTicks;   // total length in ticks
        float                                ticksPerSecond;  // playback rate (default 25 if Assimp reports 0)
        std::vector<BoneChannel>             channels;
        std::unordered_map<std::string, int> channelMap;      // boneName → index into channels, O(1) lookup
    };

}
