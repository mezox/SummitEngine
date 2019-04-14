#pragma once

#include <vector>

enum class CursorEventType
{
    Invalid,
    Press,
    Release
};

struct CursorEvent
{
    CursorEventType type;
    float x;
    float y;
}

class ICursorEventReceiver
{
public:
    virtual ~IKeyEventReceiver() = default;
    
    virtual void OnCursorEvent(CursorEvent& event) = 0;
};

class InputHandler
{
public:
    
private:
    std::vector<CursorEvent> mCursorEvents;
}
