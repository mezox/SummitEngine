#pragma once

//#include <Event/Event.h>
//
//#include <cstdint>
//
//namespace Application
//{
//    class WindowWillCloseEvent : public Event::IEvent
//    {
//    public:
//        WindowWillCloseEvent() = default;
//        ~WindowWillCloseEvent() = default;
//    };
//    
//    class WindowResizeEvent : public Event::IEvent
//    {
//    public:
//        WindowResizeEvent() = delete;
//        WindowResizeEvent(const uint16_t w, const uint16_t h)
//            : width(w), height(h)
//        {}
//        
//        ~WindowResizeEvent() = default;
//        
//    public:
//        uint16_t width{ 0 };
//        uint16_t height{ 0 };
//    };
//
//    class WindowMoveEvent : public Event::IEvent
//    {
//    public:
//        WindowMoveEvent() = delete;
//        WindowMoveEvent(const uint16_t x, const uint16_t y)
//            : originX(x), originY(y)
//        {}
//        
//        ~WindowMoveEvent() = default;
//        
//    public:
//        int16_t originX{ 0 };
//        int16_t originY{ 0 };
//    };
//}
