#pragma once

#include "DefinitelyEngine/Core.h"

#include <array>

namespace DefinitelyEngine {

    enum class HudAnchor {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center
    };

    struct HudRect {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

    struct HudVec2 {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct HudTextStyle {
        float size = 2.0f;
        std::array<float, 4> color = { 1.0f, 1.0f, 1.0f, 1.0f };
    };

}
