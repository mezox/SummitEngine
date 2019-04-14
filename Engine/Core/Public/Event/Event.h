#pragma once

#include "Signal.h"

namespace Core
{
    template<typename T>
    using Event = sigslot::signal<T>;
    
    enum class MouseEventType
    {
        Invalid,
        Move,
        Press,
        Release
    };
    
    struct MouseEvent
    {
        MouseEventType type;
        uint16_t x;
        uint16_t y;
    };
}
