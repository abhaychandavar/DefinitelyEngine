#pragma once

#include "depch.h"
#include "Event.h"

namespace DefinitelyEngine {

    class DEFINITELY_ENGINE_API KeyEvent : public Event {
    public:
        int GetKeyCode() const { return m_KeyCode; }
        int GetCategories() override { return static_cast<int>(EventCategory::Keyboard); }

    protected:
        KeyEvent(int keyCode) : m_KeyCode(keyCode) {}
        int m_KeyCode;
    };

    class DEFINITELY_ENGINE_API KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

        int GetRepeatCount() const { return m_RepeatCount; }

        EventType GetType() override { return EventType::KeyPressed; }

    protected:
        std::string GetDetailsString() override {
            return "KeyCode: " + std::to_string(m_KeyCode) + ", Repeats: " + std::to_string(m_RepeatCount);
        }

    private:
        int m_RepeatCount;
    };

    class DEFINITELY_ENGINE_API KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

        EventType GetType() override { return EventType::KeyReleased; }

    protected:
        std::string GetDetailsString() override {
            return "KeyCode: " + std::to_string(m_KeyCode);
        }
    };

}
