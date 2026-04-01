#include "depch.h"
#include "Event.h"

namespace DefinitelyEngine {
    class WindowClose: public Event {
        public:
            WindowClose() {}

            EventType GetType() {
                return EventType::WindowClose;
            }

            int GetCategories() {
                return static_cast<int>(EventCategory::Window);
            }
    };
}