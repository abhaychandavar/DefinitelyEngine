#pragma once

#include "DefinitelyEngine/Core.h"

namespace DefinitelyEngine {

enum class EventType {
    MouseMoved,
    MouseScrolled,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased
};

enum class EventCategory {
    Mouse = Bit(0),
    MouseButton = Bit(1),
    Keyboard = Bit(2)
};
    class DEFINITELY_ENGINE_API Event {
        public:
            Event() {
                this->timestamp = std::chrono::steady_clock::now();
            }

            virtual std::string ToString() {
                std::string details = this->GetDetailsString();

                return "Event[" + std::to_string((int)GetType()) + "] Categories(" +
                std::to_string(GetCategories()) + ") Time(" +
                std::to_string(GetTimestampMs()) + ")" + (details.empty() ? "" : " Details(" + details + ")");
            }

            virtual EventType GetType() = 0;

            virtual int GetCategories() = 0;

            long long GetTimestampMs() {
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                    this->timestamp.time_since_epoch()
                ).count();
            }

            bool IsInCategory(EventCategory category) {
                return this->GetCategories() & static_cast<int>(category);
            }
        
        protected:
            virtual std::string GetDetailsString() {
                return "";
            }

        private:
            std::chrono::steady_clock::time_point timestamp;
    };
}