#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

    struct Collider;  // forward-declare; full definition in Collider.h

    struct DEFINITELY_ENGINE_API Ray {
        glm::vec3 origin;
        glm::vec3 direction;  // must be normalized before use
    };

    struct DEFINITELY_ENGINE_API RaycastHit {
        float       distance = 0.0f;
        glm::vec3   point    = { 0.0f, 0.0f, 0.0f };
        glm::vec3   normal   = { 0.0f, 1.0f, 0.0f };  // default: up
        Collider*   collider = nullptr;                // non-owning
        std::string tag;                               // copy of collider->tag at hit time
    };

}
