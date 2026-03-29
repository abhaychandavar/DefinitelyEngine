#include "depch.h"
#include "Event.h"

namespace DefinitelyEngine {
    class MouseMove: public Event {
        public:
            MouseMove(Vector2 coords) {
                this->coords = coords;
            }

            EventType GetType() {
                return MOUSE_MOVED;
            }

            int GetCategories() {
                return Mouse;
            }
        
        protected:
            Vector2 coords;
    };
}