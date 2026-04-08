#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

    enum class ColliderType { Box, Capsule };

    struct DEFINITELY_ENGINE_API Collider {
        virtual ~Collider() = default;
        virtual ColliderType GetType() const = 0;

        std::string ownerName;
        glm::vec3   localOffset  = { 0.0f, 0.0f, 0.0f };  // added to transform.position (world-aligned)
        std::string tag;                                    // e.g. "Ground", "Enemy" - used to identify hits
        glm::vec3   worldCenter  = { 0.0f, 0.0f, 0.0f };  // updated by GameLayer each frame before Update()
        bool        isTrigger    = false;
    };

    // Axis-aligned box defined by half-extents in world-space metres.
    // Sizes are independent of transform.scale (which is a mesh-import artefact).
    struct DEFINITELY_ENGINE_API BoxCollider : Collider {
        ColliderType GetType() const override { return ColliderType::Box; }
        glm::vec3 halfExtents = { 0.5f, 0.5f, 0.5f };
    };

    // Upright capsule (Y-axis aligned) defined in world-space metres.
    // height = cylinder segment only; total height = height + 2 * radius.
    struct DEFINITELY_ENGINE_API CapsuleCollider : Collider {
        ColliderType GetType() const override { return ColliderType::Capsule; }
        float radius = 0.3f;
        float height = 1.8f;
    };

}
