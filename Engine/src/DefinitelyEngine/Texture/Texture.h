#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API Texture {
    public:
        virtual ~Texture() = default;

        virtual void Bind(unsigned int slot = 0) const = 0;
        virtual void Unbind() const = 0;

        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;

        static Texture* Create(const std::string& path);
    };

}