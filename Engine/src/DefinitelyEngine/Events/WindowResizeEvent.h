#pragma once

#include "depch.h"
#include "Event.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) {}

        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }

        EventType GetType() override { return EventType::WindowResize; }
        int GetCategories() override { return static_cast<int>(EventCategory::Window); }

    protected:
        std::string GetDetailsString() override {
            return "Width: " + std::to_string(m_Width) + ", Height: " + std::to_string(m_Height);
        }

    private:
        unsigned int m_Width, m_Height;
    };

}
