#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API IndexBuffer {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void SetData(unsigned int* indices, unsigned int count) = 0;
        virtual unsigned int GetCount() const = 0;

        static IndexBuffer* Create();
    };

}
