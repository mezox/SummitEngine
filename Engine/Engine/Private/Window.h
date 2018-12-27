#pragma once

#include <string>
#include <cstdint>

namespace App
{
    class Window
    {
        class NativeWindow;
        
    public:
        Window(const std::string& title, const uint16_t width, const uint16_t height);
        virtual ~Window();
            
    private:
        std::string mTitle;
        uint16_t mWidth{ 640 };
        uint16_t mHeight{ 480 };
        
        std::unique_ptr<NativeWindow> mNativeWindow;
    };
}
