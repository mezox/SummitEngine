#pragma once

#include <Event/EventBase.h>

namespace Event
{
    class EVENT_API IEvent
    {
    public:
        virtual ~IEvent() = default;
    };
}
