#pragma once

#include "DefinitelyEngine/Core.h"

enum EventType {
    MOUSE_MOVED,
    MOUSE_SCROLLED,
    KEY_PRESSED,
    KEY_RELEASED,
    MOUSE_BUTTON_PRESSED,
    MOUSE_BUTTON_RELEASED
};

enum EventCategory {
    Mouse = Bit(0),
    MouseButton = Bit(1),
    Keyboard = Bit(2)
};

namespace DefinitelyEngine {
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
                return this->GetCategories() & category;
            }
        
        protected:
            virtual std::string GetDetailsString() {
                return "";
            }

        private:
            std::chrono::steady_clock::time_point timestamp;
    };
}