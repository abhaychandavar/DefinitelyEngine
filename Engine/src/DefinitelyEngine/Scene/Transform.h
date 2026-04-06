#pragma once

#include "depch.h"

namespace DefinitelyEngine {

    struct Transform {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // Euler angles in degrees
        float     scale    = 1.0f;

        glm::mat4 GetMatrix() const {
            glm::mat4 t = glm::mat4(1.0f);
            t = glm::translate(t, position);
            t = glm::rotate(t, glm::radians(rotation.x), { 1, 0, 0 });
            t = glm::rotate(t, glm::radians(rotation.y), { 0, 1, 0 });
            t = glm::rotate(t, glm::radians(rotation.z), { 0, 0, 1 });
            t = glm::scale(t, glm::vec3(scale));
            return t;
        }
    };

}
