#include "Event.h"

namespace DefinitelyEngine {
    class MouseMove: public Event {
        public:
            MouseMove(Vector2 coords) {
                this->coords = coords;
            }

            EventType GetType() {
                return EventType(MOUSE_MOVED);
            }

            int GetCategories() {
                return EventCategory(Mouse);
            }
        
        protected:
            Vector2 coords;
    };
}