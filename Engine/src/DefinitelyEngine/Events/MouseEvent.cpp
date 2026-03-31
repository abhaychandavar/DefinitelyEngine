#include "depch.h"
#include "Event.h"

namespace DefinitelyEngine {
    class MouseMove: public Event {
        public:
            MouseMove(Vector2 coords) {
                this->coords = coords;
            }

            EventType GetType() {
                return EventType::MouseMoved;
            }

            int GetCategories() {
                return static_cast<int>(EventCategory::Mouse);
            }
        
        protected:
            Vector2 coords;
    };
}