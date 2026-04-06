#pragma once

#include "depch.h"
#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

    enum class ShaderDataType {
        None = 0,
        Float, Float2, Float3, Float4
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 8;
            case ShaderDataType::Float3: return 12;
            case ShaderDataType::Float4: return 16;
            default: return 0;
        }
    }

    struct DEFINITELY_ENGINE_API BufferElement {
        ShaderDataType Type;
        std::string    Name;
        uint32_t       Size;
        uint32_t       Offset;
        bool           Normalized;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

        uint32_t GetComponentCount() const {
            switch (Type) {
                case ShaderDataType::Float:  return 1;
                case ShaderDataType::Float2: return 2;
                case ShaderDataType::Float3: return 3;
                case ShaderDataType::Float4: return 4;
                default: return 0;
            }
        }
    };

    class DEFINITELY_ENGINE_API VertexBufferLayout {
    public:
        VertexBufferLayout() {}

        void Push(ShaderDataType type, const std::string& name, bool normalized = false) {
            BufferElement element(type, name, normalized);
            element.Offset = m_Stride;
            m_Stride += element.Size;
            m_Elements.push_back(element);
        }

        const std::vector<BufferElement>& GetElements() const { return m_Elements; }
        uint32_t GetStride() const { return m_Stride; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end()   { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end()   const { return m_Elements.end(); }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

}
