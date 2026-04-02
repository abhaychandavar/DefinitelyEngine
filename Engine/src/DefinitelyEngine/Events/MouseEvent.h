#pragma once

#include "depch.h"
#include "Event.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }

        EventType GetType() override { return EventType::MouseMoved; }
        int GetCategories() override { return static_cast<int>(EventCategory::Mouse); }

    protected:
        std::string GetDetailsString() override {
            return "X: " + std::to_string(m_X) + ", Y: " + std::to_string(m_Y);
        }

    private:
        float m_X, m_Y;
    };

    class DEFINITELY_ENGINE_API MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        EventType GetType() override { return EventType::MouseScrolled; }
        int GetCategories() override { return static_cast<int>(EventCategory::Mouse); }

    protected:
        std::string GetDetailsString() override {
            return "XOffset: " + std::to_string(m_XOffset) + ", YOffset: " + std::to_string(m_YOffset);
        }

    private:
        float m_XOffset, m_YOffset;
    };

    class DEFINITELY_ENGINE_API MouseButtonEvent : public Event {
    public:
        int GetButton() const { return m_Button; }
        int GetCategories() override {
            return static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::MouseButton);
        }

    protected:
        MouseButtonEvent(int button) : m_Button(button) {}
        int m_Button;
    };

    class DEFINITELY_ENGINE_API MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        EventType GetType() override { return EventType::MouseButtonPressed; }

    protected:
        std::string GetDetailsString() override {
            return "Button: " + std::to_string(m_Button);
        }
    };

    class DEFINITELY_ENGINE_API MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        EventType GetType() override { return EventType::MouseButtonReleased; }

    protected:
        std::string GetDetailsString() override {
            return "Button: " + std::to_string(m_Button);
        }
    };

}
