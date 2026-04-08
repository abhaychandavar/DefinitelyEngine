#pragma once

#include "depch.h"
#include "Bone.h"

namespace DefinitelyEngine {

    // A single node in the bone hierarchy tree.
    // Mirrors the aiNode structure from Assimp, but owned by us so the aiScene
    // lifetime doesn't matter after the tree is built.
    struct BoneNode {
        std::string            name;
        glm::mat4              localTransform;  // aiNode::mTransformation (bind-pose default)
        std::vector<BoneNode>  children;
    };

    // The complete skeleton data for a skinned model.
    // AnimatedModel builds one during load and exposes it read-only.
    // Animator holds a const Skeleton* — it never modifies skeleton data.
    struct Skeleton {
        BoneNode                                  rootNode;
        std::unordered_map<std::string, BoneInfo> boneInfoMap;  // boneName → id + offsetMatrix
        glm::mat4                                 globalInverseTransform;
        int                                       boneCount = 0;
    };

}
