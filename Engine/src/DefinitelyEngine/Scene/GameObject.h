#pragma once

#include "depch.h"
#include "Transform.h"

namespace DefinitelyEngine {

    struct GameObject {
        std::string name;
        Transform   transform;
        std::function<void(const glm::mat4& viewProjection, const glm::mat4& transform)> onRender;

        void Render(const glm::mat4& viewProjection) const {
            if (onRender)
                onRender(viewProjection, transform.GetMatrix());
        }
    };

}
