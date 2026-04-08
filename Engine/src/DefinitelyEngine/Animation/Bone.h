#pragma once

#include "depch.h"

namespace DefinitelyEngine {

    struct BoneInfo {
        int       id;            // index into the bone matrix array
        glm::mat4 offsetMatrix;  // transforms vertex from mesh-space to bone-space (aiBone::mOffsetMatrix)
    };

}
