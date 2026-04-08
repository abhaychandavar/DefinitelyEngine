#pragma once

#include "depch.h"
#include "Transform.h"

namespace DefinitelyEngine {

    struct Collider;  // forward-declare; full definition in Physics/Collider.h

    struct GameObject {
        std::string name;
        std::string tag;   // game-side tag, e.g. "Ground", "Enemy", "Player"
        Transform   transform;
        bool        viewSpace = false; // if true, rendered with projection only (no view) — use for FPS viewmodel objects
        Collider*   collider  = nullptr; // non-owning; lifetime managed by the owner (e.g. GameLayer)
        std::function<void(const glm::mat4& viewProjection, const glm::mat4& transform)> onRender;

        void Render(const glm::mat4& viewProjection) const {
            if (onRender)
                onRender(viewProjection, transform.GetMatrix());
        }
    };

}
